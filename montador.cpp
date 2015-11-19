#include <streambuf>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>

#define LABEL 8001
#define AND_m 0x40
#define OR_m 0x44
#define CLEAR_r1 0xB4 
#define COMP_m 0x28
#define COMPR_r1r2 0xA0
#define SHIFTR_r1r2 0xA8
#define SHIFTL_r1r2 0xA4
#define ADDR_r1r2 0x90
#define ADD_m 0x18
#define RMO_r1r2 0xAC
#define DIV_m 0x24
#define DIV_r1r2 0x9C
#define MUL_r1r2 0x98
#define MUL_m 0x20
#define SUB_m 0x1C
#define SUBR_r1r2 0x94
#define J_m 0x3C
#define JEQ 0x30
#define JGT_m 0x34
#define JLT_m 0x38
#define JSUB_m 0x48
#define RSUB 0x4C
#define LDA_m 0x00
#define LDCH_m 0x50
#define LDX_m 0x04
#define LDL_m 0x08
#define LDB_m 0x68
#define LDS_m 0x6C
#define LDT_m 0x74
#define STA_m 0x0C
#define STX_m 0x10
#define STL_m 0x14
#define STB_m 0x78
#define STS_m 0x7C
#define STT_m 0x84
#define STCH_m 0x54
#define STSW_m 0xE8
#define HLT 0xFF
#define BYTE 0xBB
#define WORD 0xAA
#define EXTDEF 0xCC
#define EXTREF 0xDD

using namespace std;

class Simbolo {
private:
    string simbolo;
    int localizacao;
    bool definida;
public:

    Simbolo(string simbolo, int localizacao, bool definida) {
        this->simbolo = simbolo;
        this->localizacao = localizacao;
        this->definida = definida;
    }

    string getSimbolo() {
        return simbolo;
    }

    int getLocalizacao() {
        return localizacao;
    }

    bool estaDefinida() {
        return definida;
    }

    void setLocalizacao(int localizacao) {
        this->localizacao = localizacao;
    }

    void setDefinida() {
        definida = true;
    }

};

class Uso {
    string simbolo;
    int localizacao;
public:

    Uso() {
        simbolo = "";
        localizacao = 0;
    }

    Uso(const string &simbolo, int localizacao) {
        this->simbolo = simbolo;
        this->localizacao = localizacao;
    }

    string getSimbolo() {
        return simbolo;
    }

    int getLocalizacao() {
        return localizacao;
    }

    void setSimbolo(const string &simbolo) {
        this->simbolo = simbolo;
    }

    void setLocalizacao(int localizacao) {
        this->localizacao = localizacao;
    }
};

class ExtDef {
    string simbolo;
    int localizacao;
public:

    ExtDef() {
        simbolo = "";
        localizacao = 0;
    }

    ExtDef(const string &simbolo, int localizacao) {
        this->simbolo = simbolo;
        this->localizacao = localizacao;
    }

    string getSimbolo() {
        return simbolo;
    }

    int getLocalizacao() {
        return localizacao;
    }

    void setSimbolo(const string &simbolo) {
        this->simbolo = simbolo;
    }

    void setLocalizacao(int localizacao) {
        this->localizacao = localizacao;
    }
};

bool temLabel(const string &label);
int ehOpcode(string leitura);
int isOperand(string operando);
void tiraEspaco(string &linha);
int tamInst(string leitura);
int regs(string registrador);

vector<Simbolo> simbolos;
vector<Uso> tabelaDeUsos;
vector<ExtDef> tabelaDeDefinicoes;

int procuraSimbolo(string simbolo);
int procuraUso(string uso);

bool comecaComChar(char c);
void listaSimbolos();
void listaTabelaDeUsos(string tabUso);
void listaTabelaDeDefinicoes(string tabDef);

void atualizaTabelaDeDefinicoes();

int montador(string arquivoEntrada, string arquivoSaida, string tabDef, string tabUso) {
    ifstream assembly(arquivoEntrada);
    fstream passo1;
    ofstream passo2(arquivoSaida);
    string linha;

    int locCounter = 0;
    int resultado;

    passo1.open("intermediario.txt", fstream::out);

    while (!assembly.eof()) {
        assembly >> linha;

        if (assembly.eof()) break;

        while (linha.front() == '.') {
            getline(assembly, linha);
            assembly >> linha;
        }

        resultado = ehOpcode(linha);
        //cout << "linha: " << linha << " resultado: " << resultado << endl;
        //cout << dec << resultado << " linha 113" << endl;

        if (resultado == -1) { //testa se eh uma label ou um opcode
            cout << "ERRO EM: " << locCounter << endl; // se nao for eh erro
            return 1;
        } else if (resultado == EXTDEF) { /*Define simbolos para serem usados em outros modulos*/
            cout << "extdef " << resultado << endl;
            getline(assembly, linha);
            tiraEspaco(linha);
            unsigned int pos = linha.find(",");
            if (pos != -1) {
                do {
                    tabelaDeDefinicoes.push_back(*new ExtDef(linha.substr(0, pos), -1));
                    linha = linha.substr(pos + 1);
                    pos = linha.find(",");
                }while (pos != -1);
                tabelaDeDefinicoes.push_back(*new ExtDef(linha.substr(0, pos), -1));
            } else {
                tabelaDeDefinicoes.push_back(*new ExtDef(linha, -1));
            }
        } else if (resultado == EXTREF) { //Uso de simbolos definidos em outros modulos
            cout << "extref " << resultado << endl; 
            getline(assembly, linha);
            tiraEspaco(linha);
            unsigned int pos = linha.find(",");
            if (pos != -1) {
                do {
                    tabelaDeUsos.push_back(*new Uso(linha.substr(0, pos), -2));
                    linha = linha.substr(pos + 1);
                    pos = linha.find(",");
                }while (pos != -1);
                tabelaDeUsos.push_back(*new Uso(linha.substr(0, pos), -2));
            } else {
                tabelaDeUsos.push_back(*new Uso(linha, -2));
            }
        } else if (resultado == LABEL) { //testa se eh label
            linha.erase(linha.size() - 1, 1);
            if (!temLabel(linha)) { // testa se ja foi definido a label
                simbolos.push_back(*new Simbolo(linha, locCounter, true)); // se nao foi, define
                cout << "Inseriu simbolo" << endl;
            } else
                cout << "ERRO" << endl; // erro = label duplicado
            //passo1 << linha << " "; //Escrevendo a label no arquivo intermediario
            //assembly >> linha; //lendo a proxima palavra
            //resultado = ehOpcode(linha);
        } else { // se nao eh label, eh opcode
            locCounter += tamInst(linha); //adiciona ao locCounter o tamanho da instrucao
            passo1 << linha << " "; //escreve a instrucao no arq intermediario
            
            /*Talvez não seja necessário o if seguinte*/
            if (linha == "BYTE") {
                getline(assembly, linha);
                locCounter += linha.size() / 2;
            } else {
                /*Pode dar 
                 problema
                 talvez
                 */
                getline(assembly, linha); //le operando(s)
                tiraEspaco(linha);
                cout << "Passou por 265: " << linha << endl;
                unsigned int pos = linha.find(",");
                if(pos == -1) {//se nao tem virgula
                    pos = procuraUso(linha);
                    if(pos == -2){ //entao existe na tabela de uso externo
                        tabelaDeUsos.push_back(*new Uso(linha, locCounter));
                    }
                }
            }
            

            unsigned int pos = linha.find(".", 0); //para comentarios depois da linha de codigo
            if (pos != -1) {
                linha = linha.substr(0, pos);
            }

            tiraEspaco(linha);

            passo1 << linha << endl;

        }


        //cout << hex << resultado << " ";
        //getline(assembly, linha);
        cout << "lcctr: " << locCounter << endl;

    }

    listaSimbolos();
    listaTabelaDeUsos(tabUso);
    listaTabelaDeDefinicoes(tabDef);
    
    atualizaTabelaDeDefinicoes();
    listaTabelaDeDefinicoes(tabDef);

    passo1.close();
    passo1.open("intermediario.txt", fstream::in);

    while (!passo1.eof()) {
        string r1, r2, m, constante;
        int tamanho, endereco;
        linha = "";
        passo1 >> linha;

        resultado = ehOpcode(linha);
        if(linha == "") break;
        tamanho = tamInst(linha);
        if (tamanho == 1) {
            if(resultado == BYTE){
                /*É byte*/
                passo1 >> hex >> constante;
                passo2 << 1;
                passo2 << " " << "00";
                passo2 << " " << hex << resultado << " " << setfill('0') << setw(2) << constante;
            }
            else{
                /* É halt*/
                passo2 << 1;
                passo2 << " " << "0";
                passo2 << " " << hex << setfill('0') << setw(2) << resultado;
            }
        } else if (tamanho == 2) {
            passo1 >> linha;
            unsigned int pos = linha.find(",", 0);
            if (pos != -1) {
                r1 = linha.substr(0, pos);
                r2 = linha.substr(pos + 1);

                int a = regs(r1), b = regs(r2);
                if (a != -1 && b != -1) {
                    passo2 << 2;
                    passo2 << " " << "000";
                    passo2 << hex << " " << setfill('0') << setw(2) << resultado << " " << a << b;
                } else {
					if(a != -1 && !comecaComChar(r2.front())){
						passo2 << 2 << hex << " " << "" << setfill('0') << setw(2) << resultado << " " << a << r2;
					}
					else
	                    cout << "Registrador inválido." << endl;
                }
            } else {
                if(resultado == CLEAR_r1){
                    int a = regs(linha);
                    if(a != -1){
                        passo2 << "2 000 " << hex << setfill('0') << setw(2) << resultado << " " << a << "0";
                    }
                    else {
                        cout << "Registrador inválido." << endl;
                    }
                }
                else{
                    cout << "Erro em instrução de tamanho 2 " << endl;
                }
            }

        } else if (tamanho == 3) {//e = 0
            /*Não estamos preocupados com o pc agora, se ele tem que pular para tras ou nao*/
            bitset<6> *nixbpe;
            if (resultado == WORD) {
                passo1 >> hex >> constante;
                passo2 << 3;
                passo2 << " " << "00";
                passo2 << " " << hex << setfill('0') << setw(2) << resultado << " " << constante;
            } else {
                passo1 >> m;
                //procura simbolo
                if (m == "") break; // e foda-se
                char c = m.front();
                switch (c) {
                    case '#':
                        m = m.substr(1); /*tira o # da frente*/
                        c = m.front();
                        if (comecaComChar(c)) {
                            /* Tem # e é um simbolo*/
                            endereco = procuraSimbolo(m);
                            if (endereco != -1) {
                                nixbpe = new bitset<6>("010010");
                                passo2 << "3 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << endereco;
                            } else {
                                int endUso = procuraUso(m);
                                if(endUso != -1){
                                    nixbpe = new bitset<6>("010010");
                                    passo2 << "3 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << "-1";
                                    //Coloca -1 para dizer que ainda nao foi definido o uso externo
                                }
                                else{
                                    cout << "Simbolo " << m << " indefinido." << endl;
                                    return 1;
                                }
                            }
                        } else {
                            /* Tem # e não é um simbolo*/
                            nixbpe = new bitset<6>("010000");
                            passo2 << "3 00 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << m;
                        }
                        break;

                    case '@':
                        m = m.substr(1); /*tira o # da frente*/
                        c = m.front();
                        if (comecaComChar(c)) {
                            /* Tem @ e é um simbolo*/
                            endereco = procuraSimbolo(m);
                            if (endereco != -1) {
                                nixbpe = new bitset<6>("100010");
                                passo2 << "3 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << endereco;
                            } else {
                                int endUso = procuraUso(m);
                                if(endUso != -1){
                                    nixbpe = new bitset<6>("010010");
                                    passo2 << "3 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << "-1";
                                    //Coloca -1 para dizer que ainda nao foi definido o uso externo
                                }
                                else{
                                    cout << "Simbolo " << m << " indefinido." << endl;
                                    return 1;
                                }
                            }
                        } else {
                            /* Tem @ e não é um simbolo*/
                            nixbpe = new bitset<6>("100000");
                            passo2 << "3 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << m;
                        }
                        break;

                    default:
                        if (comecaComChar(c)) {
                            /*Não tem @ ou # e é um simbolo*/
                            endereco = procuraSimbolo(m);
                            if (endereco != -1) {
                                nixbpe = new bitset<6>("110010");
                                passo2 << "3 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << endereco;
                            } else {
                                int endUso = procuraUso(m);
                                if(endUso != -1){
                                    nixbpe = new bitset<6>("010010");
                                    passo2 << "3 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << "-1";
                                    //Coloca -1 para dizer que ainda nao foi definido o uso externo
                                }
                                else{
                                    cout << "Simbolo " << m << " indefinido." << endl;
                                    return 1;
                                }
                            }
                        } else {
                            /*Não tem @ ou # e é um número*/
                            nixbpe = new bitset<6>("110000");
                            passo2 << "3 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << m;
                        }
                }
            }
        } else { //tamanho == 4  e = 1
            /*Não estamos preocupados com o pc agora, se ele tem que pular para tras ou nao*/
            bitset<6> *nixbpe;

            passo1 >> m;
            if (m == "") break; // e foda-se

            char c = m.front();
            switch (c) {
                case '#':
                    m = m.substr(1); /*tira o # da frente*/
                    c = m.front();
                    if (comecaComChar(c)) {
                        /* Tem # e é um simbolo*/
                        endereco = procuraSimbolo(m);
                        if (endereco != -1) {
                            nixbpe = new bitset<6>("010011");
                            passo2 << "4 01" << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << endereco;
                        } else {
                                int endUso = procuraUso(m);
                                if(endUso != -1){
                                    nixbpe = new bitset<6>("010010");
                                    passo2 << "4 01" << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << "-1";
                                    //Coloca -1 para dizer que ainda nao foi definido o uso externo
                                }
                                else{
                                    cout << "Simbolo " << m << " indefinido." << endl;
                                    return 1;
                                }
                        }
                    } else {
                        /* Tem # e não é um simbolo*/
                        nixbpe = new bitset<6>("010001");
                        passo2 << "4 00" << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << m;
                    }
                    break;

                case '@':
                    m = m.substr(1); /*tira o # da frente*/
                    c = m.front();
                    if (comecaComChar(c)) {
                        /* Tem @ e é um simbolo*/
                        endereco = procuraSimbolo(m);
                        if (endereco != -1) {
                            nixbpe = new bitset<6>("100011");
                            passo2 << "4 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << endereco;
                        } else {
                                int endUso = procuraUso(m);
                                if(endUso != -1){
                                    nixbpe = new bitset<6>("010010");
                                    passo2 << "4 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << "-1";
                                    //Coloca -1 para dizer que ainda nao foi definido o uso externo
                                }
                                else{
                                    cout << "Simbolo " << m << " indefinido." << endl;
                                    return 1;
                                }
                        }
                    } else {
                        /* Tem @ e não é um simbolo*/
                        nixbpe = new bitset<6>("100001");
                        passo2 << "4 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << m;
                    }
                    break;
                default:
                    if (comecaComChar(c)) {
                        /*Não tem @ ou # e é um simbolo*/
                        endereco = procuraSimbolo(m);
                        if (endereco != -1) {
                            nixbpe = new bitset<6>("110011");
                            passo2 << "4 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << endereco;
                        } else {
                                int endUso = procuraUso(m);
                                if(endUso != -1){
                                    nixbpe = new bitset<6>("010010");
                                    passo2 << "4 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << "-1";
                                    //Coloca -1 para dizer que ainda nao foi definido o uso externo
                                }
                                else{
                                    cout << "Simbolo " << m << " indefinido." << endl;
                                    return 1;
                                }
                        }
                    } else {
                        /*Não tem @ ou # e é um número*/
                        nixbpe = new bitset<6>("110001");
                        passo2 << "4 01 " << hex << setfill('0') << setw(2) << resultado << " " << nixbpe->to_string() << " " << m;
                    }
            }
        }
        passo2 << endl;
    }
	tabelaDeUsos.clear();
    return 0;
}
/*
string imprimeOpcode(int opcode){
    switch(opcode){
        case LDA_m:
            break;
        case LDX_m: 
            break;
        case STA_m: 
            break;
        case LDL_m:
            break;
        default:
    }
}
*/
int procuraUso(string uso) {
    if (!tabelaDeUsos.empty()) {
        for (Uso u : tabelaDeUsos) {
            if (u.getSimbolo() == uso)
                return u.getLocalizacao();
        }
        return -1;
    } else
        return -1;
}
void atualizaTabelaDeDefinicoes(){
    if(!tabelaDeDefinicoes.empty()){
        for(int i = 0; i < tabelaDeDefinicoes.size(); i++){
            int end = procuraSimbolo(tabelaDeDefinicoes[i].getSimbolo());
            if(end != -1)
                tabelaDeDefinicoes[i].setLocalizacao(end);
            else
                cout << "Erro Simbolo indefinido " << tabelaDeDefinicoes[i].getSimbolo() << endl;
        }
    }
}

bool comecaComChar(char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return true;
    else
        return false;
}

int regs(string registrador) {
    if (registrador == "A")
        return 0;
    if (registrador == "X")
        return 1;
    if (registrador == "L")
        return 2;
    if (registrador == "B")
        return 3;
    if (registrador == "S")
        return 4;
    if (registrador == "T")
        return 5;
    if (registrador == "F")
        return 6;
    if (registrador == "PC")
        return 8;
    if (registrador == "SW")
        return 9;
    return -1;
}

int procuraSimbolo(string simbolo) {

    if (!simbolos.empty()) {
        for (Simbolo s : simbolos) {
            //cout << "\tprocuraSimbolo " << s.getSimbolo() << " e" << simbolo << endl;
            if (s.getSimbolo() == simbolo)
                return s.getLocalizacao();
        }
        return -1;
    } else
        return -1;
}

bool temLabel(const string &label) {
    if (simbolos.empty())
        return false;
    for (int i = 0; i < simbolos.size(); i++) {
        if (label == simbolos[i].getSimbolo())
            return true;
        else
            return false;
    }
}

void tiraEspaco(string &linha) {
    for (int i = 0; i < linha.length(); i++) {
        while (linha[i] == ' ')
            linha.erase(i, 1);
    }
}

int tamInst(string leitura) {
    int bytes = 0;
    if (leitura.front() == '+') {
        leitura.erase(0, 1);
        bytes++;
    }
    if (leitura.compare("AND") == 0)
        return bytes += 3;
    else if (leitura.compare("OR") == 0)
        return bytes += 3;
    else if (leitura.compare("CLEAR") == 0)
        return bytes += 2;
    else if (leitura.compare("COMP") == 0)
        return bytes += 3;
    else if (leitura.compare("COMPR") == 0)
        return bytes += 2;
    else if (leitura.compare("SHIFTR") == 0)
        return bytes += 2;
    else if (leitura.compare("SHIFTL") == 0)
        return bytes += 2;
    else if (leitura.compare("ADDR") == 0)
        return bytes += 2;
    else if (leitura.compare("ADD") == 0)
        return bytes += 3;
    else if (leitura.compare("RMO") == 0)
        return bytes += 2;
    else if (leitura.compare("DIV") == 0)
        return bytes += 3;
    else if (leitura.compare("DIVR") == 0)
        return bytes += 2;
    else if (leitura.compare("MULR") == 0)
        return bytes += 2;
    else if (leitura.compare("MUL") == 0)
        return bytes += 3;
    else if (leitura.compare("SUB") == 0)
        return bytes += 3;
    else if (leitura.compare("SUBR") == 0)
        return bytes += 2;
    else if (leitura.compare("J") == 0)
        return bytes += 3;
    else if (leitura.compare("JEQ") == 0)
        return bytes += 3;
    else if (leitura.compare("JGT") == 0)
        return bytes += 3;
    else if (leitura.compare("JLT") == 0)
        return bytes += 3;
    else if (leitura.compare("JSUB") == 0)
        return bytes += 3;
    else if (leitura.compare("RSUB") == 0)
        return bytes += 3;
    else if (leitura.compare("LDA") == 0)
        return bytes += 3;
    else if (leitura.compare("LDCH") == 0)
        return bytes += 3;
    else if (leitura.compare("LDX") == 0)
        return bytes += 3;
    else if (leitura.compare("LDL") == 0)
        return bytes += 3;
    else if (leitura.compare("LDB") == 0)
        return bytes += 3;
    else if (leitura.compare("LDS") == 0)
        return bytes += 3;
    else if (leitura.compare("LDT") == 0)
        return bytes += 3;
    else if (leitura.compare("STA") == 0)
        return bytes += 3;
    else if (leitura.compare("STX") == 0)
        return bytes += 3;
    else if (leitura.compare("STL") == 0)
        return bytes += 3;
    else if (leitura.compare("STB") == 0)
        return bytes += 3;
    else if (leitura.compare("STS") == 0)
        return bytes += 3;
    else if (leitura.compare("STT") == 0)
        return bytes += 3;
    else if (leitura.compare("STCH") == 0)
        return bytes += 3;
    else if (leitura.compare("STSW") == 0)
        return bytes += 3;
    else if (leitura.compare("HLT") == 0)
        return bytes += 1;
    else if (leitura.compare("BYTE") == 0)
        return bytes += 1;
    else if (leitura.compare("WORD") == 0)
        return bytes += 3;
}

int ehOpcode(string leitura) {
    bool temMais = false;

    if (leitura.back() == ':')
        return LABEL;
    if (leitura.front() == '+') {
        leitura.erase(0, 1);
        temMais = true;
    }
    if (leitura.compare("AND") == 0)
        return AND_m;
    else if (leitura.compare("OR") == 0)
        return OR_m;
    else if (leitura.compare("CLEAR") == 0 && !temMais)
        return CLEAR_r1;
    else if (leitura.compare("COMP") == 0)
        return COMP_m;
    else if (leitura.compare("COMPR") == 0 && !temMais)
        return COMPR_r1r2;
    else if (leitura.compare("SHIFTR") == 0 && !temMais)
        return SHIFTR_r1r2;
    else if (leitura.compare("SHIFTL") == 0 && !temMais)
        return SHIFTL_r1r2;
    else if (leitura.compare("ADDR") == 0 && !temMais)
        return ADDR_r1r2;
    else if (leitura.compare("ADD") == 0)
        return ADD_m;
    else if (leitura.compare("RMO") == 0 && !temMais)
        return RMO_r1r2;
    else if (leitura.compare("DIV") == 0)
        return DIV_m;
    else if (leitura.compare("DIVR") == 0 && !temMais)
        return DIV_r1r2;
    else if (leitura.compare("MULR") == 0 && !temMais)
        return MUL_r1r2;
    else if (leitura.compare("MUL") == 0)
        return MUL_m;
    else if (leitura.compare("SUB") == 0)
        return SUB_m;
    else if (leitura.compare("SUBR") == 0 && !temMais)
        return SUBR_r1r2;
    else if (leitura.compare("J") == 0)
        return J_m;
    else if (leitura.compare("JEQ") == 0)
        return JEQ;
    else if (leitura.compare("JGT") == 0)
        return JGT_m;
    else if (leitura.compare("JLT") == 0)
        return JLT_m;
    else if (leitura.compare("JSUB") == 0)
        return JSUB_m;
    else if (leitura.compare("RSUB") == 0)
        return RSUB;
    else if (leitura.compare("LDA") == 0)
        return LDA_m;
    else if (leitura.compare("LDCH") == 0)
        return LDCH_m;
    else if (leitura.compare("LDX") == 0)
        return LDX_m;
    else if (leitura.compare("LDL") == 0)
        return LDL_m;
    else if (leitura.compare("LDB") == 0)
        return LDB_m;
    else if (leitura.compare("LDS") == 0)
        return LDS_m;
    else if (leitura.compare("LDT") == 0)
        return LDT_m;
    else if (leitura.compare("STA") == 0)
        return STA_m;
    else if (leitura.compare("STX") == 0)
        return STX_m;
    else if (leitura.compare("STL") == 0)
        return STL_m;
    else if (leitura.compare("STB") == 0)
        return STB_m;
    else if (leitura.compare("STS") == 0)
        return STS_m;
    else if (leitura.compare("STT") == 0)
        return STT_m;
    else if (leitura.compare("STCH") == 0)
        return STCH_m;
    else if (leitura.compare("STSW") == 0)
        return STSW_m;
    else if (leitura.compare("HLT") == 0 && !temMais)
        return HLT;
    else if (leitura.compare("BYTE") == 0)
        return BYTE;
    else if (leitura.compare("WORD") == 0)
        return WORD;
    else if (leitura.compare("EXTDEF") == 0)
        return EXTDEF;
    else if (leitura.compare("EXTREF") == 0)
        return EXTREF;
    else
        return -1;
}

void listaSimbolos() {
    if (!simbolos.empty())
        for (int i = 0; i < simbolos.size(); i++) {
            cout << "Simbolo: " << simbolos[i].getSimbolo() << " Localizacao: " << simbolos[i].getLocalizacao() << " Esta definido? " << simbolos[i].estaDefinida() << endl;
        }
}

void listaTabelaDeUsos(string tabUso){
    ofstream saida(tabUso);
    if(!tabelaDeUsos.empty())
        for(int i = 0; i < tabelaDeUsos.size(); i++) {
            //saida << "Uso: " << tabelaDeUsos[i].getSimbolo() << " Localizacao: " << tabelaDeUsos[i].getLocalizacao() << endl;
            saida << tabelaDeUsos[i].getSimbolo() << " " << tabelaDeUsos[i].getLocalizacao() << endl;
        }
}
void listaTabelaDeDefinicoes(string tabDef){
    ofstream saida(tabDef);
    if(!tabelaDeDefinicoes.empty())
        for(int i = 0; i < tabelaDeDefinicoes.size(); i++) {
            //saida << "Definicoes: " << tabelaDeDefinicoes[i].getSimbolo() << " Localizacao: " << tabelaDeDefinicoes[i].getLocalizacao() << endl;
            saida << tabelaDeDefinicoes[i].getSimbolo() << " " << tabelaDeDefinicoes[i].getLocalizacao() << endl;
        }
}
