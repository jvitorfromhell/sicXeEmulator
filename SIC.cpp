#include <iostream>
#include "SIC.h"

using namespace std;

int atual = 0;

SIC::SIC() {
	int i;
	for (i=0; i<10; i++) registradores[i] == 0;
}

int SIC::getReg(int index){
	return registradores[index];
}

int SIC::constroiOperando(unsigned char* memoria, int param_size) {
	int aux = 0;
	aux = memoria[atual++] << 8;
	aux = (aux | memoria[atual++]);
	if (param_size == 3)
		aux = ((aux << 8) | memoria[atual++]);
	return aux;
}

int SIC::constroiEndereco(unsigned char* memoria, int param_size, int inicial) {
	int aux = 0;
	aux = memoria[inicial++] << 8;
	aux = (aux | memoria[inicial++]);
	if (param_size == 3)
		aux = ((aux << 8) | memoria[inicial++]);
	return aux;
}

void SIC::armazenaMemoria(unsigned char* memoria, int inicial, int valor) {
	memoria[inicial] = (unsigned char) (valor & 0x00FF0000) >> 16;
	memoria[inicial+1] = (unsigned char) (valor & 0x0000FF00) >> 8;
	memoria[inicial+2] = (unsigned char) (valor & 0x000000FF);
}

int SIC::carregaAuxiliar(unsigned char* posicao, int param_size){	// Junta os tres parametros da intstrucao(um byte cada um) em um inteiro
    int aux = 0;
    aux = posicao[atual++] << 8; //zero e o mais significativo
    aux = (aux | posicao[atual++]);
	if (param_size == 3)
    	aux = ((aux << 8) | posicao[atual++]);
    return aux;
}

void SIC::gravaMemoria(unsigned char* memoria, int valor) {
    int a0, a1, a2;
    a0 = valor >> 16;
    a1 = (valor >> 8) & 0xFF;
    a2 = valor & 0xFF;
	int aux = carregaAuxiliar(memoria, 0);
    memoria[aux] = (unsigned char) a0; 
    memoria[aux + 1] = (unsigned char) a1;
    memoria[aux + 2] = (unsigned char) a2;
}

int SIC::retornaR1(unsigned char* memoria){
	  return (memoria[registradores[8]] & 0xF0) >> 4;
	
}

int SIC::retornaR2ouN(unsigned char* memoria){
    int aux = (memoria[registradores[8]] & 0x0F);
	registradores[8]++;
	return aux;
}

void SIC:: LDA_m(unsigned char* memoria, int* nixbpe){//coloca no acumulador o conteúdo da posição de memória informada
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[0] = aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[0] = aux;
		else registradores[0] = constroiEndereco(memoria, 3, aux);
	}
}

void SIC::LDCH_m(unsigned char* memoria, int* nixbpe){//coloca no acumulador o byte menos significante do conteúdo da posição de memória informada
	LDA_m(memoria, nixbpe);
	registradores[0] = registradores[0] & 0xFF;
}

void SIC::LDX_m(unsigned char* memoria, int* nixbpe){//coloca no registrador X o conteúdo da posição de memória informada
   	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[1] = aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[1] = aux;
		else registradores[1] = constroiEndereco(memoria, 3, aux);
	}
}

void SIC:: LDL_m(unsigned char* memoria, int* nixbpe){//coloca no registrador L o conteúdo da posição de memória informada
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[2] = aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[2] = aux;
		else registradores[2] = constroiEndereco(memoria, 3, aux);
	}
}

void SIC:: LDB_m(unsigned char* memoria, int* nixbpe){//coloca no registrador B o conteúdo da posição de memória informada
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[3] = aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[3] = aux;
		else registradores[3] = constroiEndereco(memoria, 3, aux);
	}
}

void SIC:: LDS_m(unsigned char* memoria, int* nixbpe){//coloca no registrador S o conteúdo da posição de memória informada
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[4] = aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[4] = aux;
		else registradores[4] = constroiEndereco(memoria, 3, aux);
	}
}

void SIC:: LDT_m(unsigned char* memoria, int* nixbpe){//coloca no registrador T o conteúdo da posição de memória informada
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[5] = aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[5] = aux;
		else registradores[5] = constroiEndereco(memoria, 3, aux);
	}
}

void SIC:: STA_m(unsigned char* memoria, int* nixbpe){//coloca na posição de memória informada o conteúdo do acumulador
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if ((n == 1 && i == 1) || (n == 0 && i == 0)) armazenaMemoria(memoria, aux, registradores[0]);
	else {
		aux = constroiEndereco(memoria, 3, aux);
		armazenaMemoria(memoria, aux, registradores[0]);
	}
}

void SIC::STX_m(unsigned char* memoria, int* nixbpe){//coloca na posição de memória informada o conteúdo do registrador X
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if ((n == 1 && i == 1) || (n == 0 && i == 0)) armazenaMemoria(memoria, aux, registradores[1]);
	else {
		aux = constroiEndereco(memoria, 3, aux);
		armazenaMemoria(memoria, aux, registradores[1]);
	}
}

void SIC::STCH_m(unsigned char* memoria, int* nixbpe){//coloca na posição de memória informada o byte menos significativo do acumulador
    int aux = registradores[0];
	registradores[0] = registradores[0] & 0x0FF;
	STA_m(memoria, nixbpe);
	registradores[0] = aux;
}

void SIC::STL_m(unsigned char* memoria, int* nixbpe){//coloca na posição de memória informada o conteúdo do registrador L
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if ((n == 1 && i == 1) || (n == 0 && i == 0)) armazenaMemoria(memoria, aux, registradores[2]);
	else {
		aux = constroiEndereco(memoria, 3, aux);
		armazenaMemoria(memoria, aux, registradores[2]);
	}
}

void SIC::STB_m(unsigned char* memoria, int* nixbpe){//coloca na posição de memória informada o conteúdo do registrador B
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if ((n == 1 && i == 1) || (n == 0 && i == 0)) armazenaMemoria(memoria, aux, registradores[3]);
	else {
		aux = constroiEndereco(memoria, 3, aux);
		armazenaMemoria(memoria, aux, registradores[3]);
	}
}

void SIC::STS_m(unsigned char* memoria, int* nixbpe){//coloca na posição de memória informada o conteúdo do registrador S
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if ((n == 1 && i == 1) || (n == 0 && i == 0)) armazenaMemoria(memoria, aux, registradores[4]);
	else {
		aux = constroiEndereco(memoria, 3, aux);
		armazenaMemoria(memoria, aux, registradores[4]);
	}
}

void SIC::STT_m(unsigned char* memoria, int* nixbpe){//coloca na posição de memória informada o conteúdo do registrador T
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if ((n == 1 && i == 1) || (n == 0 && i == 0)) armazenaMemoria(memoria, aux, registradores[5]);
	else {
		aux = constroiEndereco(memoria, 3, aux);
		armazenaMemoria(memoria, aux, registradores[5]);
	}
}

void SIC::AND_m(unsigned char* memoria, int* nixbpe){//faz uma and entre o acumulador e o conteúdo da posição de memória informada, armazena o resultado no acumulador
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[0] = registradores[0] & aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[0] = registradores[0] & aux;
		else registradores[0] = registradores[0] & constroiEndereco(memoria, 3, aux);
	}
}

void SIC:: OR_m(unsigned char* memoria, int* nixbpe){//faz uma or entre o acumulador e o conteúdo da posição de memória informada, armazena o resultado no acumulador
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[0] = registradores[0] | aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[0] = registradores[0] | aux;
		else registradores[0] = registradores[0] | constroiEndereco(memoria, 3, aux);
	}
}

void SIC:: CLEAR_r1(unsigned char* memoria){//limpa o registrador informado
	int r1 = memoria[atual++];
	r1 = r1 >> 4;
	registradores[r1] = 0;
}

void SIC:: COMP_m(unsigned char* memoria, int* nixbpe){//compara conteúdo do acumulador com o conteúdo da posição de memória indicada e seta SW de acordo(1 se maior, 0 se igual, -1 se menor)
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) aux = aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) aux = aux;
		else aux = constroiEndereco(memoria, 3, aux);
	}

    if (registradores[0] > aux)
        registradores[9] = 1; 
    if (registradores[0] == aux)
        registradores[9] = 0; 
    if(registradores[0] < aux)
        registradores[9] = -1;
}

void SIC:: COMPR_r1r2(unsigned char* memoria){//compara o conteúdo dos registradores r1 e r2 informados e seta SW de acordo(1 se maior, 0 se igual, -1 se menor)
	int aux = memoria[atual++];
	int r1 = aux >> 4;
	int r2 = aux & 0x0F;
    if(registradores[r1] > registradores[r2])
        registradores[9] = 1;
    if (registradores[r1] == registradores[r2])
        registradores[9] = 0;
    if(registradores[r1] < registradores[r2])
        registradores[9] = -1;
}

void SIC:: SHIFTR_r1n(unsigned char* memoria){ //shifta o registrador r1 informado nvezes para a direita
	int aux = memoria[atual++];
	int r = aux >> 4;
	int n = aux & 0x0F;    
	int i, x;
    for(i = 0; i < n; i++){
        x = (registradores[r] & 1);
        registradores[r] = registradores[r] >> 1;
        if (x == 1)
            registradores[r]= (registradores[r] | 0x00800000 );	//Estranho   
    }
}

void SIC:: SHIFTL_r1n(unsigned char* memoria){ //shifta o registrador r1 informado nvezes para a esquerda
	int aux = memoria[atual++];
	int r = aux >> 4;
	int n = aux & 0X0F;    
	int i, x;  
    for(i = 0; i < n; i++){
        x = (registradores[r] & 0x00800000);
	registradores[r] = registradores[r] & 0x007FFFFF;
        registradores[r] = registradores[r] << 1;
        if (x == 0x00800000)
            registradores[r] = (registradores[r] + 1);   
    }
}

void SIC:: ADDR_r1r2(unsigned char* memoria){ //soma r2 com r1 e armazena o resultado em r2
	int aux = memoria[atual++];
	int r1 = aux >> 4;
	int r2 = aux & 0X0F;
    registradores[r2] = registradores[r2] + registradores[r1];	
}

void SIC:: ADD_m(unsigned char* memoria, int* nixbpe) { //soma o conteúdo do acumulador com o conteúdo da posição de memória informado, armazena o resultado no acumulador
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[0] += aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[0] += aux;
		else registradores[0] += constroiEndereco(memoria, 3, aux);
	}
	
}

int SIC::getSinal(int valor){
    return valor >> 16;
}

int SIC::getExpoente(int valor){
    return valor & 65535;
}

void SIC:: RMO_r1r2(unsigned char* memoria) { //copiar conteúdo de r1 pra r2
	int aux = memoria[atual++];    
	int r1 = aux >> 4;
    int r2 = aux & 0X0F;
    registradores[r2] = registradores[r1];
}

void SIC:: DIV_m(unsigned char* memoria, int* nixbpe){//divide o conteúdo do acumulador por m e coloca o resultado no acumulador
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[0] /= aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[0] /= aux;
		else registradores[0] /= constroiEndereco(memoria, 3, aux);
	}
}

void SIC:: DIVF_m(unsigned char* memoria, int* nixbpe){//TODO 
	//Forte abraço!    
}

void SIC:: DIVR_r1r2(unsigned char* memoria){//divide r2 por r1 e coloca o resultado em r2
	int aux = memoria[atual++];
	int r1 = aux >> 4;
	int r2 = aux & 0x0F;
    registradores[r2] /= registradores[r1];
}

void SIC:: MUL_m(unsigned char* memoria, int* nixbpe){ //A = A * memoria
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[0] *= aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[0] *= aux;
		else registradores[0] *= constroiEndereco(memoria, 3, aux);
	}
}

void SIC:: MULF_m(unsigned char* memoria, int* nixbpe){//TODO
    //registradores.setF(registradores.getF() * memoria[indiceMemo]);
}

void SIC:: MULR_r1r2(unsigned char* memoria){//multiplica r2 e r1 e coloca o resultado em r2
	int aux = memoria[atual++];
	int r1 = aux >> 4;
	int r2 = aux & 0x0F;
    registradores[r2] *= registradores[r1];
}

void SIC:: SUB_m(unsigned char* memoria, int* nixbpe){//subtrai o conteúdo do acumulador por m e coloca o resultado no acumulador
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 0 && i == 1) registradores[0] -= aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[0] -= aux;
		else registradores[0] -= constroiEndereco(memoria, 3, aux);
	}
}

void SIC:: SUBF_m(unsigned char* memoria, int* nixbpe){//TODO
    //registradores.setF(registradores.setF() - memoria[indiceMemo]);
}

void SIC:: SUBR_r1r2(unsigned char* memoria){//subtrai r2 por r1 e coloca o resultado em r2
	int aux = memoria[atual++];
	int r1 = aux >> 4;
	int r2 = aux & 0x0F;
    registradores[r2] -= registradores[r1];
}

void SIC:: J_m(unsigned char* memoria, int* nixbpe){//PC recebe o conteúdo da posição de memória informada
	int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if (n == 1 && i == 1) registradores[8] = aux;
	else {
		aux = constroiEndereco(memoria, 3, aux);
		if ((n == 0 && i == 1) || (n == 0 && i == 0)) registradores[8] = aux;
		else registradores[8] = constroiEndereco(memoria, 3, aux);
	}
	atual = registradores[8];
}

void SIC:: JEQ_m(unsigned char* memoria, int* nixbpe){//se SW for igual a 0(representa o símbolo de igual), PC recebe o conteúdo da posição de memória informada
    if(registradores[9] == 0){ // 9 = SW
		int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
		int aux = constroiOperando(memoria, param_size);
		if (n == 0 && i == 1) registradores[8] = aux;
		else {
			aux = constroiEndereco(memoria, 3, aux);
			if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[8] = aux;
			else registradores[8] = constroiEndereco(memoria, 3, aux);
		}
	atual = registradores[8];
	}
}

void SIC:: JGT_m(unsigned char* memoria, int* nixbpe){//se SW for igual a 1(representa o símbolo de maior que), PC recebe o conteúdo da posição de memória informada
    if(registradores[9] == 1){ // 9 = SW
		int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
		int aux = constroiOperando(memoria, param_size);
		if (n == 0 && i == 1) registradores[8] = aux;
		else {
			aux = constroiEndereco(memoria, 3, aux);
			if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[8] = aux;
			else registradores[8] = constroiEndereco(memoria, 3, aux);
		}
	atual = registradores[8];
	}
}

void SIC:: JLT_m(unsigned char* memoria, int* nixbpe){//se SW for igual a -1(representa o símbolo de menor que), PC recebe o conteúdo da posição de memória informada
    if(registradores[9] == -1){ // 9 = SW
		int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
		int aux = constroiOperando(memoria, param_size);
		if (n == 0 && i == 1) registradores[8] = aux;
		else {
			aux = constroiEndereco(memoria, 3, aux);
			if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[8] = aux;
			else registradores[8] = constroiEndereco(memoria, 3, aux);
		}
	atual = registradores[8];
	}
}

void SIC:: JSUB_m(unsigned char* memoria, int* nixbpe){//não sei o que faz, mas creio que seja assim
    //registradores[2] =  registradores[8] + 3; // L <- PC
    //registradores[8] = carregaAuxiliar(memoria, 0); // PC <- m
}

void SIC:: RSUB(unsigned char* memoria){//não sei o que faz, mas creio que seja assim
    //registradores[8] = registradores[2];    // PC <- L
}

void SIC:: LDF_m(unsigned char* memoria, int* nixbpe){//TODO
	//int param_size = 2 + (nixbpe[getPC()]%2), n = nixbpe[getPC()] >> 5, i = (nixbpe[getPC()] >> 4) & 1;
	//aux = constroiOperando(memoria, param_size);
	//if (n == 0 && i == 1) registradores[0] = aux;
	//else {
	//	aux = constroiEndereco(memoria, param_size, aux);
	//	if ((n == 1 && i == 1) || (n == 0 && i == 0)) registradores[0] = aux;
	//	else registradores[0] = constroiEndereco(memoria, param_size, aux)
	//}
}

void SIC::FIX() {}

void SIC::FLOAT_instr() {}

void SIC:: STF_m(unsigned char* memoria, int* nixbpe){//TODO
    //memoria[indiceMemo] = registradores.getF();
    //memoria[indiceMemo + 1] = registradores.getFFloat();
}

void SIC:: STSW_m(unsigned char* memoria, int* nixbpe){//coloca na posição de memória informada o conteúdo do registrador SW
    int param_size = 2 + (nixbpe[getPC()]%2), n = (nixbpe[getPC()] >> 5) & 1, i = (nixbpe[getPC()] >> 4) & 1;
	int aux = constroiOperando(memoria, param_size);
	if ((n == 1 && i == 1) || (n == 0 && i == 0)) armazenaMemoria(memoria, aux, registradores[9]);
	else {
		aux = constroiEndereco(memoria, 3, aux);
		armazenaMemoria(memoria, aux, registradores[9]);
	}
}

void SIC:: executaInstrucao(unsigned char *memoria, int* nixbpe) {
    unsigned char opcode = memoria[atual++];
    //testar opcode

	if (fim) return;

	cout << hex << opcode << endl;

    switch (opcode) {
            /*Instruções Aritmeticas*/
        case 0x18:
        {
            ADD_m(memoria, nixbpe);
            break;
        }
        /*case 0x58:
        {
            ADDF_m(memoria);
            break;
        }*/
        case 0x90:
        {
            ADDR_r1r2(memoria);
            break;
        }
        case 0x24:
        {
            DIV_m(memoria, nixbpe);
            break;
        }
        case 0x64:
        {
            DIVF_m(memoria, nixbpe);
            break;
        }
        case 0x9C:
        {
            DIVR_r1r2(memoria);
            break;
        }
	case 0x20:
	{
		MUL_m(memoria, nixbpe);
		break;
	}
        case 0x60:
        {
            MULF_m(memoria, nixbpe);
            break;
        }
        case 0x98:
        {
            MULR_r1r2(memoria);
            break;
        }
        case 0x1C:
        {
            SUB_m(memoria, nixbpe);
            break;
        }
        case 0x5C:
        {
            SUBF_m(memoria, nixbpe);
            break;
        }
        case 0x94:
        {
            SUBR_r1r2(memoria);
            break;
        }

            /*Instruções Logicas*/
        case 0x40:
        {
            AND_m(memoria, nixbpe);
            break;
        }
        case 0xB4:
        {
            CLEAR_r1(memoria);
            break;
        }
        case 0x28:
        {
            COMP_m(memoria, nixbpe);
            break;
        }
        /*case 0x88:
        {
            COMPF_m(memoria);
            break;
        }*/
        case 0xA0:
        {
            COMPR_r1r2(memoria);
            break;
        }
        case 0x44:
        {
            OR_m(memoria, nixbpe);
            break;
        }
        case 0xA4:
        {
            SHIFTR_r1n(memoria);
            break;
        }
        case 0xA8:
        {
            SHIFTL_r1n(memoria);
            break;
        }
            /*Instruções de Jump*/
        case 0x3C:
        {
            J_m(memoria, nixbpe);
            break;
        }
        case 0x30:
        {
            JEQ_m(memoria, nixbpe);
            break;
        }
        case 0x34:
        {
            JGT_m(memoria, nixbpe);
            break;
        }
        case 0x38:
        {
            JLT_m(memoria, nixbpe);
            break;
        }
        case 0x48:
        {
            JSUB_m(memoria, nixbpe);
            break;
        }
        case 0x4C:
        {
            RSUB(memoria);
            break;
        }


            /*Instruções store ou load*/
        case 0x00:
        {
            LDA_m(memoria, nixbpe);
            break;
        }
        case 0x68:
        {
            LDB_m(memoria, nixbpe);
            break;
        }
        case 0x50:
        {
            LDCH_m(memoria, nixbpe);
            break;
        }
        case 0x70:
        {
            LDF_m(memoria, nixbpe);
            break;
        }
        case 0x08:
        {
            LDL_m(memoria, nixbpe);
            break;
        }
        case 0x6C:
        {
            LDS_m(memoria, nixbpe);
            break;
        }
        case 0x74:
        {
            LDT_m(memoria, nixbpe);
            break;
        }
        case 0x04:
        {
            LDX_m(memoria, nixbpe);
            break;
        }
        case 0xD0:
        {
            //LPS_m(memoria);
            break;
        }


        case 0x0C:
        {
            STA_m(memoria, nixbpe);
            break;
        }
        case 0x78:
        {
            STB_m(memoria, nixbpe);
            break;
        }
        case 0x54:
        {
            STCH_m(memoria, nixbpe);
            break;
        }
        case 0x80:
        {
            STF_m(memoria, nixbpe);
            break;
        }
        case 0xD4:
        {
            //STI_m(memoria);
            break;
        }
        case 0x14:
        {
            STL_m(memoria, nixbpe);
            break;
        }
        case 0x7C:
        {
            STS_m(memoria, nixbpe);
            break;
        }
        case 0xE8:
        {
            STSW_m(memoria, nixbpe);
            break;
        }
        case 0x84:
        {
            STT_m(memoria, nixbpe);
            break;
        }
        case 0x10:
        {
            STX_m(memoria, nixbpe);
            break;
        }
        case 0xAC:
        {
            RMO_r1r2(memoria);
            break;
        }

            /*Outras Instruções*/
        case 0xC4:
        {
            FIX();
            break;
        }
        case 0xC0:
        {
            FLOAT_instr();
            break;
        }
		case 0xFF:
		{
			fim = true;
			cout << "Uhul" << endl;
			break;
		}
    }
	registradores[8]++;
}

int SIC::getPC(){
	return registradores[8];
}
