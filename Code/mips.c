#include "common.h"
FILE * fp_mips;
extern InterCode* ir_head, * ir_tail;
extern int temp_cnt, inter_var_cnt;

ir_func* cur_func = NULL;
int* v_offset, *t_offset;
ir_func** v_func, **t_func;
ir_func* func_head = NULL;

void mips_head() {
    // 数据段
    fprintf(fp_mips, ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n");
    fprintf(fp_mips, ".globl main\n");
    // 代码段
    fprintf(fp_mips, ".text\n");
    // read
    fprintf(fp_mips, "read:\n");
    
    // 为了测试，可以先删了这行，记住交的时候加上
    fprintf(fp_mips, "  li $v0, 4\n  la $a0, _prompt\n  syscall\n"); // 提示
    
    fprintf(fp_mips, "  li $v0, 5\n  syscall\n  jr $ra\n\n"); // 读入
    // write
    fprintf(fp_mips, "write:\n");
    fprintf(fp_mips, "  li $v0, 1\n  syscall\n"); // 输出
    fprintf(fp_mips, "  li $v0, 4\n  la $a0, _ret\n  syscall\n"); // 换行
    fprintf(fp_mips, "  move $v0, $0\n  jr $ra\n"); // 返回0
}

int mips_offset(int t_or_v, int var_no, int size) {
    if(t_or_v == 1) {
        if(v_offset[var_no] != 0) {
            if(v_func[var_no] != cur_func)
                Log("v%d: %d, %s %s", var_no, v_offset[var_no], v_func[var_no]->name, cur_func->name);
            assert(v_func[var_no] == cur_func);
            return v_offset[var_no];
        }
        v_func[var_no] = cur_func;
        cur_func->var_off += size;
        v_offset[var_no] = cur_func->var_off;
        return v_offset[var_no];
    }
    else {
        if(t_offset[var_no] != 0) {
            assert(t_func[var_no] == cur_func);
            return t_offset[var_no];
        }
        t_func[var_no] = cur_func;
        cur_func->var_off += size;
        t_offset[var_no] = cur_func->var_off;
        return t_offset[var_no];
    }
}

void mips_v_t_offset(Operand* op) {
    if(op->kind == VARIABLE_T || op->kind == ADDRESS_T)
        mips_offset(0, op->var_no, 4); // 先计算偏移量
    else if(op->kind == VARIABLE_V || op->kind == ADDRESS_V)
        mips_offset(1, op->var_no, 4); // 先计算偏移量
}

void cal_offset() {
    func_head = (ir_func*)malloc(sizeof(ir_func));
    func_head->next = NULL;
    cur_func = func_head;
    InterCode* ir = ir_head;
    while(ir) {
        switch (ir->kind)
        {
            case INTER_RETURN:
            case INTER_READ:
            case INTER_WRITE:
            case INTER_ARG: { 
                mips_v_t_offset(ir->op);
                break;
            }
            case INTER_LEFT_POINTER:
            case INTER_ASSIGN: {
                mips_v_t_offset(ir->right);
                mips_v_t_offset(ir->left);
                break;
            }
            case INTER_ADD:
            case INTER_SUB:
            case INTER_MUL:
            case INTER_DIV: {
                mips_v_t_offset(ir->op1);
                mips_v_t_offset(ir->op2);
                mips_v_t_offset(ir->result);
                break;
            }
            case INTER_PARAM: {
                mips_offset(1, ir->var_no, 4);
                break;
            }
            case INTER_IF_GOTO: {
                mips_v_t_offset(ir->if_goto.op1);
                mips_v_t_offset(ir->if_goto.op2);
                break;
            }
            case INTER_FUNCTION: {
                ir_func* new_func = (ir_func*)malloc(sizeof(ir_func));
                cur_func->next = new_func;
                cur_func = new_func;
                cur_func->n_param = 0;
                cur_func->var_off = 8;
                cur_func->name = ir->func_name;
                break;
            }
            case INTER_CALL: {
                mips_v_t_offset(ir->ret);
                break;
            }
            case INTER_DEC: {
                mips_offset(1, ir->dec.var_no, ir->dec.width);
                break;
            }
            default: {
                break;
            }
        }
        ir = ir->next;
    }
}

void gen_mips() {
    v_offset = (int*)malloc(sizeof(int) * (inter_var_cnt + 1));
    memset(v_offset, 0, sizeof(int) * (inter_var_cnt + 1));
    t_offset = (int*)malloc(sizeof(int) * (temp_cnt + 1));
    memset(t_offset, 0, sizeof(int) * (temp_cnt + 1));
    v_func = (ir_func**)malloc(sizeof(ir_func*) * (inter_var_cnt + 1));
    memset(v_func, 0, sizeof(ir_func*) * (inter_var_cnt + 1));
    t_func = (ir_func**)malloc(sizeof(ir_func*) * (temp_cnt + 1));
    memset(t_func, 0, sizeof(ir_func*) * (temp_cnt + 1));
    mips_head();
    cal_offset();

    cur_func = func_head;
    InterCode* ir = ir_head;
    while(ir) {
        mips_text(ir);
        ir = ir->next;
    }
}


void mips_ld(Operand* op, int reg_no) {
    if(op->pre == NOTHING) { // x
        if(op->kind == CONSTANT) {
            fprintf(fp_mips, "  li $t%d, %d\n", reg_no, op->int_value);
        }
        else if (op->kind == VARIABLE_T || op->kind == ADDRESS_T)
        {
            assert(t_offset[op->var_no] != 0);
            fprintf(fp_mips, "  lw $t%d, %d($fp)\n", reg_no, -t_offset[op->var_no]);
        }
        else if (op->kind == VARIABLE_V || op->kind == ADDRESS_V)
        {
            assert(v_offset[op->var_no] != 0);
            fprintf(fp_mips, "  lw $t%d, %d($fp)\n", reg_no, -v_offset[op->var_no]);
        }
    }
    else if(op->pre == PRE_AND) { // &x
        if (op->kind == ADDRESS_T)
        {
            assert(t_offset[op->var_no] != 0);
            fprintf(fp_mips, "  la $t%d, %d($fp)\n", reg_no, -t_offset[op->var_no]);
        }
        else if (op->kind == ADDRESS_V)
        {
            assert(v_offset[op->var_no] != 0);
            fprintf(fp_mips, "  la $t%d, %d($fp)\n", reg_no, -v_offset[op->var_no]);
        }
        else {
            printf("%d %d\n", op->kind, op->var_no);
            assert(0);
        }
    }
    else { // *x
        if (op->kind == VARIABLE_T)
        {
            assert(t_offset[op->var_no] != 0);
            fprintf(fp_mips, "  lw $t%d, %d($fp)\n", reg_no, -t_offset[op->var_no]); // lw reg1 b
            fprintf(fp_mips, "  lw $t%d, 0($t%d)\n", reg_no, reg_no);// lw reg1 0(reg1)
        }
        else if (op->kind == VARIABLE_V)
        {
            assert(v_offset[op->var_no] != 0);
            fprintf(fp_mips, "  lw $t%d, %d($fp)\n", reg_no, -v_offset[op->var_no]); // lw reg1 b
            fprintf(fp_mips, "  lw $t%d, 0($t%d)\n", reg_no, reg_no);// lw reg1 0(reg1)
        }
        else {
            printf("%d %d\n", op->kind, op->var_no);
            assert(0);
        }
    }
}

void mips_st(Operand* op, int reg_no){
    if(op->pre != NOTHING) {
        printf("%d %d %d\n", op->pre, op->kind, op->var_no);
        assert(0);
    }
    if(op->kind == VARIABLE_T || op->kind == ADDRESS_T) {
        assert(t_offset[op->var_no] != 0);
        mips_offset(0, op->var_no, 4);
        fprintf(fp_mips, "  sw $t%d, %d($fp)\n", reg_no, -t_offset[op->var_no]);
    }
    else if (op->kind == VARIABLE_V || op->kind == ADDRESS_V)
    {
        assert(v_offset[op->var_no] != 0);
        mips_offset(1, op->var_no, 4);
        fprintf(fp_mips, "  sw $t%d, %d($fp)\n", reg_no, -v_offset[op->var_no]);
    }
    else {
        printf("%d %d\n", op->kind, op->var_no);
        TODO();
    }
}

void mips_text(InterCode* ir) {
    switch (ir->kind)
    {
        case INTER_ASSIGN: {
            mips_ld(ir->right, 1);
            mips_st(ir->left, 1);
            break;
        }
        case INTER_ADD: {
            // x := y + z -> add reg(x), reg(y), reg(z)
            mips_ld(ir->op1, 1);
            mips_ld(ir->op2, 2);
            fprintf(fp_mips, "  add $t1, $t1, $t2\n");
            mips_st(ir->result, 1);
            break;
        }
        case INTER_SUB: {
            // x := y - z -> sub reg(x), reg(y), reg(z)
            mips_ld(ir->op1, 1);
            mips_ld(ir->op2, 2);
            fprintf(fp_mips, "  sub $t1, $t1, $t2\n");
            mips_st(ir->result, 1);
            break;
        }    
        case INTER_MUL: {
            // x := y + z -> mul reg(x), reg(y), reg(z)
            mips_ld(ir->op1, 1);
            mips_ld(ir->op2, 2);
            fprintf(fp_mips, "  mul $t1, $t1, $t2\n");
            mips_st(ir->result, 1);
            break;
        }
        case INTER_DIV: {
            // x := y + z -> div reg(y), reg(z)
            // mflo reg(x)
            mips_ld(ir->op1, 1);
            mips_ld(ir->op2, 2);
            fprintf(fp_mips, "  div $t1, $t2\n");
            fprintf(fp_mips, "  mflo $t1\n");
            mips_st(ir->result, 1);
            break;
        }
        case INTER_FUNCTION: {
            // f:
            cur_func = cur_func->next;
            if(strcmp(ir->func_name, "main") == 0)
                fprintf(fp_mips, "\n%s:\n", ir->func_name);
            else 
                fprintf(fp_mips, "\nfunc_%s:\n", ir->func_name);
            fprintf(fp_mips, "  addi $fp, $sp, 8\n"); // $fp = $sp + 8
            
            fprintf(fp_mips, "  li $t1, -%d\n", cur_func->var_off); 
            fprintf(fp_mips, "  add $sp, $fp, $t1\n"); // 为这个函数中的局部变量临时变量申请栈中的空间
            // fprintf(fp_mips, "  addi $sp, $fp, %d\n", cur_func->var_off);
            break;
        }
        case INTER_CALL: {
            fprintf(fp_mips, "  addi $sp, $sp, -8\n");
            fprintf(fp_mips, "  sw $ra, 0($sp)\n");// 保存原$ra至栈中
            fprintf(fp_mips, "  sw $fp, 4($sp)\n");// 保存原$fp至栈中
            if(strcmp(ir->func_name, "main") == 0)
                fprintf(fp_mips, "  jal %s\n", ir->func_name);
            else    
                fprintf(fp_mips, "  jal func_%s\n", ir->func_name);// jal f (jal会自动把下个指令的地址存到$ra中)
            fprintf(fp_mips, "  lw $fp, 4($sp)\n"); // 从栈中恢复原$fp
            fprintf(fp_mips, "  lw $ra, 0($sp)\n"); // 从栈中恢复原$ra
            fprintf(fp_mips, "  addi $sp, $sp, 8\n"); // 此时$sp = $fp
            fprintf(fp_mips, "  add $sp, $sp, $v1\n"); // $v1存的是这次调用中参数占的空间，减掉以后栈就复原到调用前了
            fprintf(fp_mips, "  move $t1, $v0\n");
            mips_st(ir->ret, 1); // 存返回值
            break;
        }
        case INTER_ARG: { // ARG x 
            // 注意ARG是倒序的，比如f(1, 2, 3), 是先ARG $3
            // 为了简洁一点，不考虑效率的话就全部压栈而不存寄存器了
            mips_ld(ir->op, 1); // $t1 = x
            fprintf(fp_mips, "  addi $sp, $sp, -4\n");
            fprintf(fp_mips, "  sw $t1, 0($sp)\n");// 参数压栈
            break;
        }
        case INTER_PARAM: { // PARAM x
            fprintf(fp_mips, "  lw $t1, %d($fp)\n", 4 * cur_func->n_param); // 从栈里找到参数
            cur_func->n_param ++;
            int off = mips_offset(1, ir->var_no, 4);
            fprintf(fp_mips, "  sw $t1, %d($fp)\n", -off);// 把参数存到x
            break;
        }
        case INTER_RETURN: {
            // move $v0, reg(x)
            // jr $ra
            mips_ld(ir->op, 1); // 返回值放到$t1
            fprintf(fp_mips, "  move $v0, $t1\n"); // 返回值放到$v0
            fprintf(fp_mips, "  addi $sp, $fp, -8\n"); // sp = fp - 8, 即消除函数内局部变量和临时变量的影响
            fprintf(fp_mips, "  li $v1, %d\n", cur_func->n_param * 4); // $v1存参数占的空间
            fprintf(fp_mips, "  jr $ra\n");
            break;
        }
        case INTER_READ: {
            fprintf(fp_mips, "  addi $sp, $sp, -4\n");
            fprintf(fp_mips, "  sw $ra, 0($sp)\n");
            fprintf(fp_mips, "  jal read\n");
            fprintf(fp_mips, "  lw $ra, 0($sp)\n");
            fprintf(fp_mips, "  addi $sp, $sp, 4\n");
            fprintf(fp_mips, "  move $t1, $v0\n");
            mips_st(ir->op, 1);
            break;
        }
        case INTER_WRITE: {
            mips_ld(ir->op, 1); // lw $t1 x
            fprintf(fp_mips, "  move $a0, $t1\n"); // move a0 x
            fprintf(fp_mips, "  addi $sp, $sp, -4\n"); // addi $sp, $sp, -4
            fprintf(fp_mips, "  sw $ra, 0($sp)\n"); // sw $ra 0($sp)
            fprintf(fp_mips, "  jal write\n"); // jal write
            fprintf(fp_mips, "  lw $ra, 0($sp)\n"); // lw $ra 0($sp)
            fprintf(fp_mips, "  addi $sp, $sp, 4\n"); // addi $sp, $sp, 4
            fprintf(fp_mips, "  move $v0, $0\n"); // move $v0 $0
            break;
        }
        case INTER_LABEL: {
            // LABEL x: -> x:
            fprintf(fp_mips, "label%d:\n", ir->label);
            break;
        }
        case INTER_GOTO: {
            // j x
            fprintf(fp_mips, "  j label%d\n", ir->label);
            break;
        }
        case INTER_IF_GOTO: {
            // beq reg(x), reg(y), label
            mips_ld(ir->if_goto.op1, 1);
            mips_ld(ir->if_goto.op2, 2);
            
            if(ir->if_goto.relop == 0) // <
                fprintf(fp_mips, "  blt $t1, $t2, label%d\n", ir->if_goto.label);
            else if(ir->if_goto.relop == 1) // >
                fprintf(fp_mips, "  bgt $t1, $t2, label%d\n", ir->if_goto.label);
            else if(ir->if_goto.relop == 2) // !=
                fprintf(fp_mips, "  bne $t1, $t2, label%d\n", ir->if_goto.label);
            else if(ir->if_goto.relop == 3) // ==
                fprintf(fp_mips, "  beq $t1, $t2, label%d\n", ir->if_goto.label);
            else if(ir->if_goto.relop == 4) // <=
                fprintf(fp_mips, "  ble $t1, $t2, label%d\n", ir->if_goto.label);
            else if(ir->if_goto.relop == 5) // >=
                fprintf(fp_mips, "  bge $t1, $t2, label%d\n", ir->if_goto.label);
            else 
                assert(0);
            break;
        }
        case INTER_DEC: { // 之前用了
            break;
        }
        case INTER_LEFT_POINTER: {
            // *x := y -> sw reg(y), 0(reg(x))
            mips_ld(ir->right, 1);
            mips_ld(ir->left, 2);
            fprintf(fp_mips, "  sw $t1, 0($t2)\n");
            break;
        }
        default: {
            assert(0);
            break;
        }
    }
}