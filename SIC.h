#ifndef SIC_H
#define	SIC_H

class SIC {
private:
    int registradores[10];

public:
    void executaInstrucao(unsigned char* memoria, int* nixbpe);
	void gravaMemoria(unsigned char*memoria, int valor);
	int getReg(int index);
	int getPC();
	SIC();
	bool fim = false;
    
private:    
	
    int carregaAuxiliar(unsigned char*posicao, int param_size);
    void gravaMemoria(int valor);
    int retornaR1(unsigned char* memoria);
    int retornaR2ouN(unsigned char* memoria);

	int constroiEndereco(unsigned char* memoria, int param_size, int inicial);
	int constroiOperando(unsigned char* memoria, int param_size);
	void armazenaMemoria(unsigned char* memoria, int inicial, int valor);
    
    int getSinal(int valor);
    int getExpoente(int valor);
    
    void AND_m(unsigned char* memoria, int* nixbpe);
    void OR_m(unsigned char* memoria, int* nixbpe);
    void CLEAR_r1(unsigned char*memoria);
    void COMP_m(unsigned char* memoria, int* nixbpe);
    //void COMPF_m(float indiceMemo);
    void COMPR_r1r2(unsigned char*memoria);
    void SHIFTR_r1n(unsigned char* memoria);
    void SHIFTL_r1n(unsigned char* memoria);
    
    void ADDR_r1r2(unsigned char*memoria);
    void ADD_m(unsigned char* memoria, int* nixbpe) ;
    void ADDF_m(unsigned char* memoria, int* nixbpe);
    void SUBR_r1r2(unsigned char*memoria);
    void DIVR_r1r2(unsigned char*memoria);
    void RMO_r1r2(unsigned char*memoria);            
    void DIV_m(unsigned char* memoria, int* nixbpe);
    void DIVF_m(unsigned char* memoria, int* nixbpe);
    void MUL_m(unsigned char* memoria, int* nixbpe);
    void MULF_m(unsigned char* memoria, int* nixbpe);
    void MULR_r1r2(unsigned char*memoria);
    void SUB_m(unsigned char* memoria, int* nixbpe);
    void SUBF_m(unsigned char* memoria, int* nixbpe);
    
    void J_m(unsigned char* memoria, int* nixbpe);
    void JEQ_m(unsigned char* memoria, int* nixbpe);
    void JGT_m(unsigned char* memoria, int* nixbpe);
    void JLT_m(unsigned char* memoria, int* nixbpe);
    void JSUB_m(unsigned char* memoria, int* nixbpe);
    void RSUB(unsigned char* memoria);
    void FIX();
    void FLOAT_instr();
    void LDA_m(unsigned char* memoria, int* nixbpe);
    void LDB_m(unsigned char* memoria, int* nixbpe);
    void LDCH_m(unsigned char* memoria, int* nixbpe);
    void LDF_m(unsigned char* memoria, int* nixbpe);
    void LDL_m(unsigned char* memoria, int* nixbpe);
    void LDS_m(unsigned char* memoria, int* nixbpe);
    void LDT_m(unsigned char* memoria, int* nixbpe);
    void LDX_m(unsigned char* memoria, int* nixbpe);
    void LPS_m(unsigned char* memoria, int* nixbpe); //não implementei
    void STA_m(unsigned char* memoria, int* nixbpe);
    void STB_m(unsigned char* memoria, int* nixbpe);
    void STCH_m(unsigned char* memoria, int* nixbpe);
    void STF_m(unsigned char* memoria, int* nixbpe);
    void STI_m(unsigned char* memoria, int* nixbpe); //não implementei
    void STL_m(unsigned char* memoria, int* nixbpe);
    void STS_m(unsigned char* memoria, int* nixbpe);
    void STSW_m(unsigned char* memoria, int* nixbpe);
    void STT_m(unsigned char* memoria, int* nixbpe);
    void STX_m(unsigned char* memoria, int* nixbpe);
	
};

#endif	/* SIC_H */

