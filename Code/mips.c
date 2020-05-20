#include "common.h"
FILE * fp_mips;
extern InterCode* ir_head, * ir_tail;
extern int temp_cnt, inter_var_cnt;

void mips_data() {
    int* var_sz = (int*)malloc((inter_var_cnt + 1) * sizeof(int));
    memset(var_sz, 0, (inter_var_cnt + 1) * sizeof(int));
    
    InterCode* ir = ir_head;
    while(ir) {
        if(ir->kind == INTER_DEC) {
            var_sz[ir->dec.var_no] = ir->dec.width;
        }
        ir = ir->next;
    }
    for(int i = 1; i <= inter_var_cnt; ++i) {
        if(var_sz[i])
            fprintf(fp_mips, "_v%d: .space %d\n", i, var_sz[i]);
        else 
            fprintf(fp_mips, "_v%d: .word 0\n", i);
    }
    for(int i = 1; i <= temp_cnt; ++i) {
        fprintf(fp_mips, "_t%d: .word 0\n", i);
    }
}

void mips_head() {
    // 数据段
    fprintf(fp_mips, ".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n");
    mips_data();
    fprintf(fp_mips, ".globl main\n");
    // 代码段
    fprintf(fp_mips, ".text\n");
    fprintf(fp_mips, "read:\n  li $v0, 4\n  la $a0, _prompt\n  syscall\n  li $v0, 5\n  syscall\n  jr $ra\n\n");
    fprintf(fp_mips, "write:\n  li $v0, 1\n  syscall\n  li $v0, 4\n  la $a0, _ret\n  syscall\n  move $v0, $0\n  jr $ra\n");
}

void gen_mips() {
    mips_head();
    InterCode* ir = ir_head;
    while(ir) {
        mips_text(ir);
        ir = ir->next;
    }
}

// int getReg(Operand* op) {
// }

void mips_ld(Operand* op, int no) {
    if(op->pre == NOTHING) { // x
        if(op->kind == CONSTANT) {
            fprintf(fp_mips, "  li $t%d, %d\n", no, op->int_value);
        }
        else if (op->kind == VARIABLE_T || op->kind == ADDRESS_T)
        {
            fprintf(fp_mips, "  lw $t%d, _t%d\n", no, op->var_no);
        }
        else if (op->kind == VARIABLE_V || op->kind == ADDRESS_V)
        {
            fprintf(fp_mips, "  lw $t%d, _v%d\n", no, op->var_no);
        }
    }
    else if(op->pre == PRE_AND) { // &x
        if (op->kind == VARIABLE_T || op->kind == ADDRESS_T)
        {
            fprintf(fp_mips, "  la $t%d, _t%d\n", no, op->var_no);
        }
        else if (op->kind == VARIABLE_V || op->kind == ADDRESS_V)
        {
            fprintf(fp_mips, "  la $t%d, _v%d\n", no, op->var_no);
        }
        else {
            assert(0);
        }
    }
    else { // *x
        if (op->kind == VARIABLE_T || op->kind == ADDRESS_T)
        {
            fprintf(fp_mips, "  lw $t%d, _t%d\n", no, op->var_no); // lw reg1 b
            fprintf(fp_mips, "  lw $t%d, 0($t%d)\n", no, no);// lw reg1 0(reg1)
        }
        else if (op->kind == VARIABLE_V || op->kind == ADDRESS_V)
        {
            fprintf(fp_mips, "  lw $t%d, _v%d\n", no, op->var_no); // lw reg1 b
            fprintf(fp_mips, "  lw $t%d, 0($t%d)\n", no, no);// lw reg1 0(reg1)
        }
        else {
            assert(0);
        }
    }
}

void mips_st(Operand* op, int no){
    if(op->kind == VARIABLE_T) {
        fprintf(fp_mips, "  sw $t%d, _t%d\n", no, op->var_no);
    }
    else if (op->kind == VARIABLE_V)
    {
        fprintf(fp_mips, "  sw $t%d, _v%d\n", no, op->var_no);
    }
    else {
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
            fprintf(fp_mips, "\n%s:\n", ir->func_name);
            break;
        }
        case INTER_CALL: {
            // jal f
            // move reg(x), $v0
            TODO();
            // fprintf(fp_mips, "  jal f\n", ir->func_name);
            // fprintf(fp_mips, "  move $t1, $v0\n");
            // mips_st(ir->ret, 1);
            break;
        }
        case INTER_ARG: {
            TODO();
            break;
        }
        case INTER_PARAM: {
            TODO();
            break;
        }
        case INTER_RETURN: {
            // move $v0, reg(x)
            // jr $ra
            mips_ld(ir->op, 1);
            fprintf(fp_mips, "  move $v0, $t1\n");
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
            fprintf(fp_mips, "  sw $ra 0($sp)\n"); // sw $ra 0($sp)
            fprintf(fp_mips, "  jal write\n"); // jal write
            fprintf(fp_mips, "  lw $ra 0($sp)\n"); // lw $ra 0($sp)
            fprintf(fp_mips, "  addi $sp, $sp, 4\n"); // addi $sp, $sp, 4
            fprintf(fp_mips, "  move $v0 $0\n"); // move $v0 $0
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
                fprintf(fp_mips, "   blt $t1, $t2, label%d\n", ir->if_goto.label);
            else if(ir->if_goto.relop == 1) // >
                fprintf(fp_mips, "   bgt $t1, $t2, label%d\n", ir->if_goto.label);
            else if(ir->if_goto.relop == 2) // !=
                fprintf(fp_mips, "   bne $t1, $t2, label%d\n", ir->if_goto.label);
            else if(ir->if_goto.relop == 3) // ==
                fprintf(fp_mips, "   beq $t1, $t2, label%d\n", ir->if_goto.label);
            else if(ir->if_goto.relop == 4) // <=
                fprintf(fp_mips, "   ble $t1, $t2, label%d\n", ir->if_goto.label);
            else if(ir->if_goto.relop == 5) // >=
                fprintf(fp_mips, "   bge $t1, $t2, label%d\n", ir->if_goto.label);
            else 
                assert(0);
            break;
        }
        case INTER_DEC: { // 前面用过了
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