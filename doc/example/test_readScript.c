/******************************************************
 Script interpreter code for language test 
 Generated by Script-to-Bytecode's LanguageCompiler
 Modified for a sample execution (just decompiles the script commands)
******************************************************/
#include <stdlib.h>
#include <stdio.h>
#include "test_lang.h"

typedef struct {
    unsigned char * data;
    unsigned offset, size;
    }
test_script_t;

test_script_t test_getScript(unsigned target, unsigned offset)
{
    char filename[11]; 
    sprintf(filename, "%d.test", target);
	FILE* scriptf = fopen(filename, "r");
    fseek(scriptf, 0, SEEK_END);
    size_t size = ftell(scriptf) - offset;
    fseek(scriptf, offset, SEEK_SET);
    unsigned char * data = malloc(size);
    fread(data, 1, size, scriptf);
    
    test_script_t ret;
    ret.data = data;
    ret.size = size;
    ret.offset = offset;
    return ret;
}

unsigned char * test_getStringAt(test_script_t script, unsigned offset)
{
    return &(script.data[offset-script.offset]);
}

unsigned char test_readScriptByteAt(test_script_t script, unsigned offset)
{
	return script.data[offset-script.offset];
}

unsigned test_readScriptAt(test_script_t script, unsigned offset, unsigned char width)
{
	//little endian
	unsigned ret = 0, i;
	for (i=0; i<width; i++)
		ret += test_readScriptByteAt(script, offset+i) << (i<<3);
	return ret;
}

void printarray(unsigned a[], unsigned size)
{
    unsigned i;
    for(i=0; i<size; i++)
        printf(" 0x%x", a[i]);
}

unsigned test_runScript(unsigned target, unsigned offset)
{
	test_script_t script = test_getScript(target, offset);
    unsigned char continueRunning = 1;
	unsigned parameter[test_MAXNUMPARAMS];
    while(continueRunning) {
		unsigned commandCode = test_readScriptAt(script, offset, test_CODEWIDTH);
		offset += test_CODEWIDTH;
		switch (commandCode) {
			case test_CMD_nop:
				printf("nop");
				break; //nop

			case test_CMD_end:
				printf("end");
				break; //end
                
			case test_CMD_system:
				printf("system");
				break; //end

			case test_CMD_set:
				parameter[0] = test_readScriptAt(script, offset + test_POS_set_0, test_WID_set_0);
				parameter[1] = test_readScriptAt(script, offset + test_POS_set_1, test_WID_set_1);
				printf("set"); printarray(parameter, 2);
				offset += test_LEN_set;
				break; //set

			case test_CMD_cp:
				parameter[0] = test_readScriptAt(script, offset + test_POS_cp_0, test_WID_cp_0);
				parameter[1] = test_readScriptAt(script, offset + test_POS_cp_1, test_WID_cp_1);
				printf("cp"); printarray(parameter, 2);
				offset += test_LEN_cp;
				break; //cp

			case test_CMD_add:
				parameter[0] = test_readScriptAt(script, offset + test_POS_add_0, test_WID_add_0);
				parameter[1] = test_readScriptAt(script, offset + test_POS_add_1, test_WID_add_1);
				parameter[2] = test_readScriptAt(script, offset + test_POS_add_2, test_WID_add_2);
				printf("add"); printarray(parameter, 3);
				offset += test_LEN_add;
				break; //add

			case test_CMD_addi:
				parameter[0] = test_readScriptAt(script, offset + test_POS_addi_0, test_WID_addi_0);
				parameter[1] = test_readScriptAt(script, offset + test_POS_addi_1, test_WID_addi_1);
				parameter[2] = test_readScriptAt(script, offset + test_POS_addi_2, test_WID_addi_2);
				printf("addi"); printarray(parameter, 3);
				offset += test_LEN_addi;
				break; //addi

			case test_CMD_sub:
				parameter[0] = test_readScriptAt(script, offset + test_POS_sub_0, test_WID_sub_0);
				parameter[1] = test_readScriptAt(script, offset + test_POS_sub_1, test_WID_sub_1);
				parameter[2] = test_readScriptAt(script, offset + test_POS_sub_2, test_WID_sub_2);
				printf("sub"); printarray(parameter, 3);
				offset += test_LEN_sub;
				break; //sub

			case test_CMD_subi:
				parameter[0] = test_readScriptAt(script, offset + test_POS_subi_0, test_WID_subi_0);
				parameter[1] = test_readScriptAt(script, offset + test_POS_subi_1, test_WID_subi_1);
				parameter[2] = test_readScriptAt(script, offset + test_POS_subi_2, test_WID_subi_2);
				printf("subi"); printarray(parameter, 3);
				offset += test_LEN_subi;
				break; //subi

			case test_CMD_print:
				parameter[0] = test_readScriptAt(script, offset + test_POS_goin_0, test_WID_goin_0);
				printf("print"); printarray(parameter, 1);
                printf(" (\"%s\")", test_getStringAt(script, parameter[0]));
				offset += test_LEN_goin;
				break; //print
                
			case test_CMD_goto:
				parameter[0] = test_readScriptAt(script, offset + test_POS_goto_0, test_WID_goto_0);
				parameter[1] = test_readScriptAt(script, offset + test_POS_goto_1, test_WID_goto_1);
				printf("goto"); printarray(parameter, 2);
				offset += test_LEN_goto;
				break; //goto

			case test_CMD_goin:
				parameter[0] = test_readScriptAt(script, offset + test_POS_goin_0, test_WID_goin_0);
				printf("goin"); printarray(parameter, 1);
				offset += test_LEN_goin;
				break; //goin

			case test_CMD_biz:
				parameter[0] = test_readScriptAt(script, offset + test_POS_biz_0, test_WID_biz_0);
				parameter[1] = test_readScriptAt(script, offset + test_POS_biz_1, test_WID_biz_1);
				printf("biz"); printarray(parameter, 2);
				offset += test_LEN_biz;
				break; //biz

			case test_CMD_binz:
				parameter[0] = test_readScriptAt(script, offset + test_POS_binz_0, test_WID_binz_0);
				parameter[1] = test_readScriptAt(script, offset + test_POS_binz_1, test_WID_binz_1);
				printf("binz"); printarray(parameter, 2);
				offset += test_LEN_binz;
				break; //binz

			case test_CMD_push:
				parameter[0] = test_readScriptAt(script, offset + test_POS_push_0, test_WID_push_0);
				parameter[1] = test_readScriptAt(script, offset + test_POS_push_1, test_WID_push_1);
				printf("push"); printarray(parameter, 2);
				offset += test_LEN_push;
				break; //push

			case test_CMD_pop:
				printf("pop");
				break; //pop

			case test_CMD_return:
				printf("return");
				break; //return

			default: printf("??? (0x%x) \'%c\'", commandCode, commandCode);
                break;
		} //end switch (commandCode)
		printf(";\n");
        continueRunning = offset < script.size;
	}
    free(script.data);
	return offset;
}



int main (int argc, char* argv[])
{
    if (argc != 3){
        printf("USAGE: %s <script> <offset>", argv[0]);
        }
    else {
        test_runScript(atoi(argv[1]), atoi(argv[2]));
        }
}
