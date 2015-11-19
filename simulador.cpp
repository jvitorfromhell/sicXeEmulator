#include <gtkmm.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include "simulador.h"
#include "SIC.h"

using namespace std;

Simulador::Simulador(int nArq) : 
	executa("Executar Programa"), 
	passo("Passo a Passo"),
	macros("Processar Macros"),
	montar("Montar Código(s)"),
	ligar("Linkar Código(s)"),
	carregar("Carregar Programa"),
	vDiv(Gtk::ORIENTATION_VERTICAL),
	macroDiv(Gtk::ORIENTATION_VERTICAL),
	montaDiv(Gtk::ORIENTATION_VERTICAL),
	hDiv1(Gtk::ORIENTATION_HORIZONTAL),
	hDiv2(Gtk::ORIENTATION_HORIZONTAL),
	hDiv3(Gtk::ORIENTATION_HORIZONTAL),
	botoes(Gtk::ORIENTATION_HORIZONTAL),
	memTitulo("Posição de Memória"),
	memTituloVal("Valor Armazenado"),
	nArq(nArq)
{	
	int i;

	set_title("Simulador SIC/XE");
	set_default_size(640, 480);
	set_position(Gtk::WIN_POS_CENTER);

	ifstream entradaCodigo1("modulo1.txt");
	stringstream strCodigo1;
	strCodigo1 << entradaCodigo1.rdbuf();
	entradaCodigo1.close();

	bufferInicial1 = Gtk::TextBuffer::create();	
	bufferInicial1->set_text(strCodigo1.str());
	inicial1.set_buffer(bufferInicial1);

	if (nArq == 2) {
		ifstream entradaCodigo2("modulo2.txt");
		stringstream strCodigo2;
		strCodigo2 << entradaCodigo2.rdbuf();

		bufferInicial2 = Gtk::TextBuffer::create();
		bufferInicial2->set_text(strCodigo2.str());
		inicial2.set_buffer(bufferInicial2);
	}

	inicial1.set_cursor_visible(false);
	inicial1.set_editable(false);
	inicial2.set_cursor_visible(false);
	inicial2.set_editable(false);

	janela_inicial1.add(inicial1);
	janela_inicial1.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	janela_inicial1.set_shadow_type(Gtk::SHADOW_IN);
	janela_inicial2.add(inicial2);
	janela_inicial2.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	janela_inicial2.set_shadow_type(Gtk::SHADOW_IN);

	macroDiv.set_position(400/nArq);
	macroDiv.add1(janela_inicial1);
	macroDiv.add2(janela_inicial2);

	montado1.set_cursor_visible(false);
	montado1.set_editable(false);
	montado2.set_cursor_visible(false);
	montado2.set_editable(false);

	janela_montado1.add(montado1);
	janela_montado1.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	janela_montado1.set_shadow_type(Gtk::SHADOW_IN);
	janela_montado2.add(montado2);
	janela_montado2.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	janela_montado2.set_shadow_type(Gtk::SHADOW_IN);

	montaDiv.set_position(400/nArq);
	montaDiv.add1(janela_montado1);
	montaDiv.add2(janela_montado2);

	hDiv1.set_position(285);
	hDiv1.add1(macroDiv);
	hDiv1.add2(montaDiv);

	codigoFinal.set_cursor_visible(false);
	codigoFinal.set_editable(false);

	janela_codigo.add(codigoFinal);
	janela_codigo.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	janela_codigo.set_shadow_type(Gtk::SHADOW_IN);

	registradores.set_row_homogeneous(true);
	registradores.set_column_homogeneous(true);
	registradores.set_column_spacing(50);
	registradores.set_row_spacing(10);
	
	reg[0].set_text("R0 - A");
	reg[1].set_text("R1 - X");
	reg[2].set_text("R2 - L");
	reg[3].set_text("R3 - B");
	reg[4].set_text("R4 - S");
	reg[5].set_text("R5 - T");
	reg[6].set_text("R6 - F (High)");
	reg[7].set_text("R7 - F (Low)");
	reg[8].set_text("R8 - PC");
	reg[9].set_text("R9 - SW");

	for(i=0; i<10; i++) {
		regVal[i].set_text("0");
		registradores.attach(reg[i], 0, i, 1, 1);
		registradores.attach(regVal[i], 1, i, 1, 1);
	}
	
	memoria.set_row_homogeneous(true);
	memoria.set_column_homogeneous(true);
	memoria.set_column_spacing(10);
	memoria.set_row_spacing(10);

	memoria.attach(memTitulo, 0, 0, 1, 1);
	memoria.attach(memTituloVal, 1, 0, 1, 1);

	janela_memoria.add(memoria);
	janela_memoria.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	janela_memoria.set_shadow_type(Gtk::SHADOW_IN);

	mem_reg.append_page(registradores, "Registradores");
	mem_reg.append_page(janela_memoria, "Memória");

	hDiv2.set_position(300);
	hDiv2.add1(janela_codigo);
	hDiv2.add2(mem_reg);

	botoes.set_homogeneous(true);
	botoes.set_spacing(0);
	botoes.set_border_width(20);
	botoes.pack_start(macros, Gtk::PACK_EXPAND_PADDING, 20);
	botoes.pack_start(montar, Gtk::PACK_EXPAND_PADDING, 20);
	botoes.pack_start(ligar, Gtk::PACK_EXPAND_PADDING, 20);
	botoes.pack_start(carregar, Gtk::PACK_EXPAND_PADDING, 20);
	botoes.pack_start(passo, Gtk::PACK_EXPAND_PADDING, 20);
	botoes.pack_start(executa,Gtk::PACK_EXPAND_PADDING, 20);

	hDiv3.set_position(600);
	hDiv3.add1(hDiv1);
	hDiv3.add2(hDiv2);

	vDiv.set_border_width(10);
	vDiv.set_position(400);
	vDiv.add1(hDiv3);
	vDiv.add2(botoes);

	add(vDiv);
	
	macros.signal_clicked().connect(sigc::mem_fun(*this, &Simulador::click_macros));
	montar.signal_clicked().connect(sigc::mem_fun(*this, &Simulador::click_montar));
	ligar.signal_clicked().connect(sigc::mem_fun(*this, &Simulador::click_ligar));
	carregar.signal_clicked().connect(sigc::mem_fun(*this, &Simulador::click_carregar));
	executa.signal_clicked().connect(sigc::mem_fun(*this, &Simulador::click_executa));
	passo.signal_clicked().connect(sigc::mem_fun(*this, &Simulador::click_passo));
	montar.set_sensitive(false);
	ligar.set_sensitive(false);
	carregar.set_sensitive(false);
	passo.set_sensitive(false);
	executa.set_sensitive(false);

	bufferErro = Gtk::TextBuffer::create();

	show_all_children();
}

Simulador::~Simulador() {}

void Simulador::click_macros() {
	int resultado;
	string codigo1("modulo1.txt"), codigoExp1("codigoExp1.txt");
	resultado = processadorMacro(codigo1, codigoExp1);
	if (resultado != 0) {
		if (resultado == 2) 
			bufferErro->set_text("\tErro no Processador de Macro: \n\tChamada de Macro Inexistente");
		else
			bufferErro->set_text("\tErro no Processador de Macro: \n\tMacro não Definida"); 
		montado1.set_buffer(bufferErro);	
		return; 
	}

	ifstream processada1("codigoExp1.txt");
	stringstream macro1;
	macro1 << processada1.rdbuf();
	processada1.close();

	bufferMacro1 = Gtk::TextBuffer::create();	
	bufferMacro1->set_text(macro1.str());
	inicial1.set_buffer(bufferMacro1);

	if (nArq == 2) {
		string codigo2("modulo2.txt"), codigoExp2("codigoExp2.txt");
		resultado = processadorMacro(codigo2, codigoExp2);
		if (resultado != 0) {
			if (resultado == 2) 
				bufferErro->set_text("\tErro no Processador de Macro: \n\tChamada de Macro Inexistente");
			else
				bufferErro->set_text("\tErro no Processador de Macro: \n\tMacro não Definida");
			montado1.set_buffer(bufferErro);
			return; 
		}

		ifstream processada2("codigoExp2.txt");
		stringstream macro2;
		macro2 << processada2.rdbuf();
		processada2.close();

		bufferMacro2 = Gtk::TextBuffer::create();	
		bufferMacro2->set_text(macro2.str());
		inicial2.set_buffer(bufferMacro2);
	}

	macros.set_sensitive(false);
	montar.set_sensitive(true);
}

void Simulador::click_montar() {
	string codigoFonte1("codigoExp1.txt"), codigoMontado1("codigoMontado1.txt"), definicoes1("definicoes1.txt"), uso1("uso1.txt");
	int resultado;
	resultado = montador(codigoFonte1, codigoMontado1, definicoes1, uso1);

	if (resultado != 0) {
		bufferErro->set_text("\tErro de Montagem");
		montado1.set_buffer(bufferErro);
		return;
	}

	ifstream mont1("codigoMontado1.txt");
	stringstream assembled1;
	assembled1 << mont1.rdbuf();
	mont1.close();

	bufferMontado1 = Gtk::TextBuffer::create();	
	bufferMontado1->set_text(assembled1.str());
	montado1.set_buffer(bufferMontado1);

	if (nArq == 2) {
		string codigoFonte2("codigoExp2.txt"), codigoMontado2("codigoMontado2.txt"), definicoes2("definicoes2.txt"), uso2("uso2.txt");
		resultado = montador(codigoFonte2, codigoMontado2, definicoes2, uso2);

		if (resultado != 0) {
			bufferErro->set_text("\tErro de Montagem");
			montado2.set_buffer(bufferErro);
			return;
		}

		ifstream mont2("codigoMontado2.txt");
		stringstream assembled2;
		assembled2 << mont2.rdbuf();
		mont2.close();

		bufferMontado2 = Gtk::TextBuffer::create();	
		bufferMontado2->set_text(assembled2.str());
		montado2.set_buffer(bufferMontado2);
	}

	montar.set_sensitive(false);
	ligar.set_sensitive(true);
}

void Simulador::click_ligar() {
	string ultimoArquivo;
	int resultado;

	if (nArq == 1) 	ultimoArquivo = "codigoMontado1.txt";
	else {
		ligador("codigoMontado1.txt", "codigoMontado2.txt", "uso1.txt", "uso2.txt", "definicoes1.txt", "definicoes2.txt", "codigoLinkado.txt");
		ultimoArquivo = "codigoLinkado.txt";
	}

	tag1 = Gtk::TextBuffer::Tag::create();
	tag1->property_background() = "yellow";

	tags = Gtk::TextBuffer::TagTable::create();
	tags->add(tag1);

	ifstream codigoFinal(ultimoArquivo);
	stringstream codigoLinkado;
	codigoLinkado << codigoFinal.rdbuf();
	codigoFinal.close();

	bufferFinal = Gtk::TextBuffer::create(tags);
	bufferFinal->set_text(codigoLinkado.str());
	bufferFinal->apply_tag(tag1, bufferFinal->begin(), bufferFinal->get_iter_at_line(1)); 
	this->codigoFinal.set_buffer(bufferFinal);

	ligar.set_sensitive(false);	
	carregar.set_sensitive(true);
}

void Simulador::click_carregar() {
	if (nArq == 1) carregador("codigoMontado1.txt");
	else carregador("codigoLinkado.txt");	

	if (memIndex <= 50) memIndex = 50;

	for (int i=0; i < memIndex; i++)
		setarMem(i, memSimulador[i]);	

	carregar.set_sensitive(false);	
	passo.set_sensitive(true);
	executa.set_sensitive(true);
}

void Simulador::click_passo() {
	bufferFinal->remove_tag(tag1, bufferFinal->get_iter_at_line(processador.getPC()), bufferFinal->get_iter_at_line(processador.getPC()+1));
	processador.executaInstrucao(memSimulador, nixbpe);
	update_registradores();
	update_memoria();
	bufferFinal->apply_tag(tag1, bufferFinal->get_iter_at_line(processador.getPC()), bufferFinal->get_iter_at_line(processador.getPC()+1));
	mark = bufferFinal->create_mark(bufferFinal->get_iter_at_line(processador.getPC()+5));
	codigoFinal.scroll_to(mark);
}

void Simulador::click_executa() {
	while (!processador.fim) {
		click_passo();	
	}
}

int charToHex(char letra){
	if(letra >= '0' && letra <= '9')
		letra -= '0';
	else if (letra >= 65 && letra <= 70)
		letra -= 55;
		 else
		letra -= 87;

return letra;
}

void Simulador::carregador(string arquivoLinkado) {
	ifstream codigoObjeto(arquivoLinkado);
	int tamInstrucao, realoc, aux;
	unsigned char c1, c2, *help, opcode;
	string linhaAtual;

	while (!codigoObjeto.eof()) {
		codigoObjeto >> tamInstrucao;
		codigoObjeto >> realoc;
		codigoObjeto >> c1;
		c1 = charToHex(c1);
		codigoObjeto >> c2;
		c2 = charToHex(c2);
		opcode = (c1 << 4) | c2;

		switch (tamInstrucao) {
			case 1:	if (opcode == 0xbb){
						cout << "ENTREI" << endl;
						codigoObjeto >> c1;
						c1 = charToHex(c1);
						codigoObjeto >> c2;
						c2 = charToHex(c2);
						memSimulador[memIndex++] = (c1 << 4) | c2;
					}
					else memSimulador[memIndex++] = opcode;
					getline(codigoObjeto, linhaAtual);
					linhaAtual.erase(linhaAtual.begin(), linhaAtual.end());					
					nixbpe[instIndex++] = 9999;
					break;

			case 2:	memSimulador[memIndex++] = opcode;
					codigoObjeto >> c1;
					c1 = charToHex(c1);
					codigoObjeto >> c2;
					c2 = charToHex(c2);
					memSimulador[memIndex++] = (c1 << 4) | c2;
					nixbpe[instIndex++] = 9999;
					getline(codigoObjeto, linhaAtual);
					linhaAtual.erase(linhaAtual.begin(), linhaAtual.end());
					break;

			case 3:	if (opcode == 0xaa){
						getline(codigoObjeto, linhaAtual);
						linhaAtual.erase(0, 1);
						aux = linhaAtual.size();
						help = (unsigned char *) malloc (aux * sizeof(unsigned char));
						for (int i=0; i < aux; i++) {
							help[i] = charToHex(linhaAtual.front());
							linhaAtual.erase(0, 1);
						}
						switch (aux) {
							case 0: memSimulador[memIndex++] = 0;
									memSimulador[memIndex++] = 0;
									memSimulador[memIndex++] = 0;
									break;

							case 1: memSimulador[memIndex++] = 0;
									memSimulador[memIndex++] = 0;
									memSimulador[memIndex++] = help[0];
									break;

							case 2: memSimulador[memIndex++] = 0;
									memSimulador[memIndex++] = 0;
									memSimulador[memIndex++] = (help[0] << 4) | help[1];
									break;

							case 3:	memSimulador[memIndex++] = 0;
									memSimulador[memIndex++] = help[0];
									memSimulador[memIndex++] = (help[1] << 4) | help[2];
									break;

							case 4:	memSimulador[memIndex++] = 0;
									memSimulador[memIndex++] = (help[0] << 4) | help[1];
									memSimulador[memIndex++] = (help[2] << 4) | help[3];
									break;

							case 5:	memSimulador[memIndex++] = help[0];
									memSimulador[memIndex++] = (help[1] << 4) | help[2];
									memSimulador[memIndex++] = (help[3] << 4) | help[4];
									break;

							case 6:	memSimulador[memIndex++] = (help[0] << 4) | help[1];
									memSimulador[memIndex++] = (help[2] << 4) | help[3];
									memSimulador[memIndex++] = (help[4] << 4) | help[5];
									break;
							}
						nixbpe[instIndex++] = 9999;						
						}
						else {
							memSimulador[memIndex++] = opcode;
							codigoObjeto >> nixbpe[instIndex++];
							getline(codigoObjeto, linhaAtual);
							linhaAtual.erase(0, 1);
							aux = linhaAtual.size();
							help = (unsigned char *) malloc (aux * sizeof(unsigned char));
							for (int i = 0; i < aux; i++) {
								help[i] = charToHex(linhaAtual.front());
								linhaAtual.erase(0, 1);
							}
							switch (aux) {
								case 1: memSimulador[memIndex++] = 0;
										memSimulador[memIndex++] = help[0];
										break;
	
								case 2: memSimulador[memIndex++] = 0;
										memSimulador[memIndex++] = (help[0] << 4) | help [1];
										break;

								case 3: memSimulador[memIndex++] = help[0];
										memSimulador[memIndex++] = (help[1] << 4) | help [2];
										break;

								case 4: memSimulador[memIndex++] = (help[0] << 4) | help [1];
										memSimulador[memIndex++] = (help[2] << 4) | help [3];
										break;
							}
						}
						free(help);
						linhaAtual.erase(linhaAtual.begin(), linhaAtual.end());
						break;
					
			case 4: memSimulador[memIndex++] = opcode;
					codigoObjeto >> nixbpe[instIndex++];
					getline(codigoObjeto, linhaAtual);
					linhaAtual.erase(0, 1);
					aux = linhaAtual.size();
					help = (unsigned char *) malloc (aux * sizeof(unsigned char));
					for (int i=0; i < aux; i++) {
							help[i] = charToHex(linhaAtual.front());
							linhaAtual.erase(0, 1);
					}
					switch(aux) {
						case 1: memSimulador[memIndex++] = 0;
								memSimulador[memIndex++] = 0;
								memSimulador[memIndex++] = help[0];
								break;

						case 2: memSimulador[memIndex++] = 0;
								memSimulador[memIndex++] = 0;
								memSimulador[memIndex++] = (help[0] << 4) | help[1];
								break;

						case 3:	memSimulador[memIndex++] = 0;
								memSimulador[memIndex++] = help[0];
								memSimulador[memIndex++] = (help[1] << 4) | help[2];
								break;

						case 4:	memSimulador[memIndex++] = 0;
								memSimulador[memIndex++] = (help[0] << 4) | help[1];
								memSimulador[memIndex++] = (help[2] << 4) | help[3];
								break;

						case 5:	memSimulador[memIndex++] = help[0];
								memSimulador[memIndex++] = (help[1] << 4) | help[2];
								memSimulador[memIndex++] = (help[3] << 4) | help[4];
								break;

						case 6:	memSimulador[memIndex++] = (help[0] << 4) | help[1];
								memSimulador[memIndex++] = (help[2] << 4) | help[3];
								memSimulador[memIndex++] = (help[4] << 4) | help[5];
								break;
					}
					free(help);
					linhaAtual.erase(linhaAtual.begin(), linhaAtual.end());
					break;
			}
	}
	memIndex--;
	memSimulador[memIndex] = 0;
}

void Simulador::setarMem(int pos, int val) {
	ostringstream posicaoTabela, valorPosicao;
	posicaoTabela << pos;
	valorPosicao << val;
	mem[pos].set_text(posicaoTabela.str());
	memVal[pos].set_text(valorPosicao.str());
	if (pos == 8000) memVal[pos].set_text("DALE XAVANTE!");
	memoria.attach(mem[pos], 0, pos+1, 1, 1);		
	memoria.attach(memVal[pos], 1, pos+1, 1, 1);
	show_all_children();
}

void Simulador::update_registradores(){
	for(int i=0; i<10; i++)
		regVal[i].set_text(to_string(processador.getReg(i)));
}

void Simulador::update_memoria(){
	for (int i=0; i<memIndex; i++)
		memVal[i].set_text(to_string(memSimulador[i]));
}
