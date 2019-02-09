#include<iostream>
#include<vector>
#include<string>
#include<cstdlib>
#include<sstream>
#include<set>
#include<stack>
#include<queue>
#include<conio.h>
#include<fstream>
#include<stdio.h>
#include<map>
using namespace std;

struct trans {
	int vertice_de;
	int vertice_a;
	char trans_simbolo;
};
 
int nRenAFD=0, nColAFD=0;
map<string, set<string> > Primeros;
map<string, set<string> > Siguientes;
map<string, bool> AuxRec;
set<string>Terminales;
set<string>NoTerminales;
vector<string>Lexemas_allr;
vector<string>Lexemas_alnom;
 //Reglas semanticas
map<int, vector<string> >reglas;
map<int, string>TradsRed;
 
int ren_tas=0, col_tas=0;
string **tabla_analisis = NULL;
  
class AFN {
	public:
		vector<int> vertice;
		vector<trans> transiciones;
		int estado_final;
	AFN() { }
	int get_contar_vertices() { return vertice.size(); }
	void set_vertice(int nuevo_vertice) {
		for(int i = 0; i < nuevo_vertice; i++)
			vertice.push_back(i);
	}
	void set_transiciones( int vertice_de, int vertice_a, char trans_simbolo ) {
		trans new_trans;
		new_trans.vertice_de = vertice_de;
		new_trans.vertice_a = vertice_a;
		new_trans.trans_simbolo = trans_simbolo;
		transiciones.push_back(new_trans);
	}
	void set_estado_final( int fs ) { estado_final = fs; }
	int get_estado_final() { return estado_final; }
};

// funcion para obtener el alfabeto 
string ObtenerAlfabeto( string er ){
	string alf="", t;
	string a, b;
	int cont =0;
	for( string::iterator it = er.begin(); it != er.end(); ++it ) {
		a=*it;
		if(a != "(" && a != "'" && a != ")" && a != "*" && a != "|" && a != "." && a != "?" && a != "+"){
 			if(cont<1){ alf=alf+a; cont++; }
			else{
			 	int band=0;
 				for( string::iterator it2 = alf.begin(); it2 != alf.end(); ++it2 ){
					b=*it2;
 					if( a==b )
 						band++;
 				}
 				if(!band) {alf=alf+a; }
 			}
 		}
 	}
 	return alf+"^"; //agregar la cadena vacia
}
  
string **inicializaMatriz( int ren, int col ){
	string **p= new string*[ren];
	for( int i=0; i<ren; i++  ){
		p[i]= new string[col];
	}
	for( int i=0; i<ren; i++ ){
		for( int j=0; j<col; j++ ){
			p[i][j]="-";
		}
	}
	return p;
}

void printMatrizArchivoAFN( string **a, int ren, int col ){
 	ofstream tt("Tabla de Transiciones AFN.txt");
	for( int i=0; i<ren; i++ ){
		for( int j=0; j<col; j++ )
			tt << a[i][j] << "\t" ;
		tt<<endl;
	}
	tt.close();
 }
 
 void rellenaMatriz( string **a, int ren, int col, AFN af, string alf ){
 	map< char,int > nColumna;
 	int cont=1;
	for(string::iterator it = alf.begin() ; it != alf.end(); ++it ){
		nColumna[*it]=cont;
		cont++;
	}
 	int i=1;
 	for( string::iterator it = alf.begin(); it != alf.end(); ++it ){
 		char k[10];
 		k[0]=*it;
 		string l(k);	//transformo el char en string
 		a[0][i]=l;
 		i++;
 	}
 	for( int i=1; i<af.get_contar_vertices()+1; i++ ){
 		string cad="";
 		cad = static_cast<ostringstream*>(&(ostringstream() << i))->str(); //convierto los numeros de filas a char para la tabla
 		a[i][0]=cad;
 	}
	a[0][0]="E.";
	trans new_trans;
	for( int i = 0; i < af.transiciones.size(); i++ ){
		new_trans = af.transiciones.at(i);
		for( map<char, int>::iterator it=nColumna.begin(); it!=nColumna.end(); ++it ){
			char c[10];
			c[0] = it->first;
			int nc = it->second;
			if( new_trans.trans_simbolo==c[0] ){
				if( a[new_trans.vertice_de+1][it->second]=="-" ){
					string cad="";
 					cad = static_cast<ostringstream*>(&(ostringstream() << (new_trans.vertice_a+1)))->str();
					a[new_trans.vertice_de+1][it->second]=cad;
				}
				else{
					string cad="";
 					cad = ","+static_cast<ostringstream*>(&(ostringstream() << (new_trans.vertice_a+1)))->str();
					a[new_trans.vertice_de+1][it->second]=a[new_trans.vertice_de+1][it->second]+cad;
				}
			}
		}
	}
 }
 
void liberaMatriz( string ***a, int ren ){
	for(int i=0; i<ren; i++)
		delete [] (*a)[i];
	delete []*a;
	*a=NULL;
}
 
string **getTable( AFN a, string er, string alf, int ren, int col ){
	string **tablatransicion = NULL;
	tablatransicion = inicializaMatriz( ren, col );
	rellenaMatriz( tablatransicion, ren, col, a, alf );
	printMatrizArchivoAFN( tablatransicion, ren, col );
	return tablatransicion;
}

/**
 * Algoritmos para la contruccion del automata a partir de la expresion regular(er.txt)
 * 
 * Se hace de la siguiente manera.
 * Las relgas correspondientes a las operaciones entre expresones regulares son:
 *	Todas las operaciones por tipo deben ir en bloques de parentesis.
 * 	(r.s) 	--->	Concatenaci·n(que es lo mismo que (rs) (·sta expresi·n sin el . no es aceptada por el programa), 
 * 							no se acepta igual a (r(s|a)) como una concatenaci·n, se debe marcar como . ) 
 * 	(r|s)	--->	Uni·n(que es lo mismo que (r+s)(·sta espresi·n no es aceptada por el programa)).
 * 	(r*),(r+),(r?) ->	Klenne 
 * 
 * Ejemplos de expresiones regulares:
 * er=((a.f.g)|H), er=(a.b), er=((((a.b.f)*)|(w|a|f))) 
 * 
 * 
*/

AFN kleene( AFN a ){
	AFN resultante;
	int i;
	trans new_trans;
	resultante.set_vertice( a.get_contar_vertices() + 2 );
	resultante.set_transiciones( 0, 1, '^' );
	for( i = 0; i < a.transiciones.size(); i++ ){
		new_trans = a.transiciones.at( i );
		resultante.set_transiciones( new_trans.vertice_de + 1, new_trans.vertice_a + 1, new_trans.trans_simbolo );
	}
	resultante.set_transiciones( a.get_contar_vertices(), a.get_contar_vertices() + 1, '^' );
	resultante.set_transiciones( a.get_contar_vertices(), 1, '^' );
	resultante.set_transiciones( 0, a.get_contar_vertices() + 1, '^' );
	resultante.set_estado_final( a.get_contar_vertices() + 1 );
	return resultante;
}
 
AFN kleenePositiva( AFN a ){
	AFN resultante;
	int i;
	trans new_trans;
	resultante.set_vertice( a.get_contar_vertices() + 2 );
	resultante.set_transiciones( 0, 1, '^' );
	for( i = 0; i < a.transiciones.size(); i++ ){
		new_trans = a.transiciones.at( i );
		resultante.set_transiciones( new_trans.vertice_de + 1, new_trans.vertice_a + 1, new_trans.trans_simbolo );
	}
	resultante.set_transiciones( a.get_contar_vertices(), a.get_contar_vertices() + 1, '^' );
	resultante.set_transiciones( a.get_contar_vertices(), 1, '^' );
	resultante.set_estado_final( a.get_contar_vertices() + 1 );
	return resultante;
}
 
AFN kleeneOpcional(AFN a) {
	AFN resultante;
	int i;
	trans new_trans;
	resultante.set_vertice(a.get_contar_vertices() + 2);
	resultante.set_transiciones(0, 1, '^');
	for(i = 0; i < a.transiciones.size(); i++) {
		new_trans = a.transiciones.at(i);
		resultante.set_transiciones(new_trans.vertice_de + 1, new_trans.vertice_a + 1, new_trans.trans_simbolo);
	}
	resultante.set_transiciones(a.get_contar_vertices(), a.get_contar_vertices() + 1, '^');
	resultante.set_transiciones(0, a.get_contar_vertices() + 1, '^');
	resultante.set_estado_final(a.get_contar_vertices() + 1);
	return resultante;
}
 
AFN concat(AFN a, AFN b) {
	AFN resultante;
	resultante.set_vertice(a.get_contar_vertices() + b.get_contar_vertices()-1);
	int i;
	trans new_trans;
	for(i = 0; i < a.transiciones.size(); i++) {
		new_trans = a.transiciones.at(i);
		resultante.set_transiciones(new_trans.vertice_de, new_trans.vertice_a, new_trans.trans_simbolo);
	}
	for(i = 0; i < b.transiciones.size(); i++) {
		new_trans = b.transiciones.at(i);
		resultante.set_transiciones(new_trans.vertice_de + a.get_contar_vertices()-1, new_trans.vertice_a + a.get_contar_vertices()-1, new_trans.trans_simbolo);
	}
	resultante.set_estado_final(a.get_contar_vertices() + b.get_contar_vertices() - 1);
	return resultante;
}
 
AFN or_seleccion(vector<AFN> selecciones, int no_de_selecciones) {
	AFN resultante;
	int contar_vertices = 2;
	int i, j;
	AFN med;
	trans new_trans;
	for(i = 0; i < no_de_selecciones; i++) {
		contar_vertices += selecciones.at(i).get_contar_vertices();
	}
	resultante.set_vertice(contar_vertices);
	int cont = 1;
	for(i = 0; i < no_de_selecciones; i++) {
		resultante.set_transiciones(0, cont, '^');
		med = selecciones.at(i);
		for(j = 0; j < med.transiciones.size(); j++) {
			new_trans = med.transiciones.at(j);
			resultante.set_transiciones(new_trans.vertice_de + cont, new_trans.vertice_a + cont, new_trans.trans_simbolo);
		}
		cont += med.get_contar_vertices();
		resultante.set_transiciones(cont - 1, contar_vertices - 1, '^');
	}
	resultante.set_estado_final(contar_vertices - 1);
	return resultante;
}
 
AFN er_a_afn(string er) {
	stack<char> operadores; //pila de acciones a hacer con las letras del abecedario ya convertidas en AFN
	stack<AFN> operandos;   //son todos los operandos de la expresion regular.
	char op_simb, cur_simb;
	int op_suma;
	AFN *new_simb;	
	for(string::iterator it = er.begin(); it != er.end(); ++it) {
		cur_simb = *it;	//apuntador *it a la expresi·n regular para obtener los caracteres... simbolo en curso.
		if(cur_simb != '(' && cur_simb != ')' && cur_simb != '*' && cur_simb != '|' && cur_simb != '.' && cur_simb != '?' && cur_simb != '+') {
			new_simb = new AFN();
			new_simb->set_vertice(2);
			new_simb->set_transiciones(0, 1, cur_simb);
			new_simb->set_estado_final(1);
			operandos.push(*new_simb);
			delete new_simb;
		} 
		else {
			if(cur_simb == '*') {
				AFN star_simb = operandos.top();
				operandos.pop();
				operandos.push(kleene(star_simb));
			} 
			else {
				if(cur_simb == '+') {
					AFN star_simb = operandos.top();
					operandos.pop();
					operandos.push(kleenePositiva(star_simb));
				} 
				else {
					if(cur_simb == '?') {
						AFN star_simb = operandos.top();
						operandos.pop();
						operandos.push(kleeneOpcional(star_simb));
					} 
					else {
						if(cur_simb == '.') {
							operadores.push(cur_simb);
						} 
						else{
						
							if(cur_simb == '|') {
								operadores.push(cur_simb);
							} 
							else {
								if(cur_simb == '(') {
									operadores.push(cur_simb);
								} 
								else {
									op_suma = 0;
									char c;
									op_simb = operadores.top();
									if(op_simb == '(') continue;
									do {
										operadores.pop();
										op_suma++;
									} while(operadores.top() != '(');
									operadores.pop();
									AFN op1; AFN op2;
									vector<AFN> selecciones;
									if(op_simb == '.') {
										for(int i = 0; i < op_suma; i++) {
											op2 = operandos.top();
											operandos.pop();
											op1 = operandos.top();
											operandos.pop();
											operandos.push(concat(op1, op2));
										}
									} else{
										if(op_simb == '|'){
											selecciones.assign(op_suma + 1, AFN());
											int tracker = op_suma;
											for(int i = 0; i < op_suma + 1; i++) {
												selecciones.at(tracker) = operandos.top();
												tracker--;
												operandos.pop();
											}
											operandos.push(or_seleccion(selecciones, op_suma+1));
										} 
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return operandos.top();
}

string setTostring(set<int> estados){
 	string cad="";
	int cont=0;
 	for(set<int>::iterator it =estados.begin(); it!=estados.end(); ++it){
 		int n=*it;
 		if(cont==0){
 			cad = static_cast<ostringstream*>(&(ostringstream() << n))->str(); //convierto los numeros de filas a char para la tabla
 			cont++;
 		}
 		else{ cad = cad + "," + static_cast<ostringstream*>(&(ostringstream() << n))->str(); }
 	}
 	return cad;
}
 
set<int> stringToset(string temp){
	set<int> estados;
	string cad="";
	for(string::iterator it =temp.begin(); it!=temp.end(); ++it){
	 	char c; 
		c=*it;
 		if(*it == ','){
 			estados.insert(atoi(cad.c_str()));
			cad="";
 		}
 		else{ cad=cad+c; }
 		if(it==temp.end()-1){
 			estados.insert(atoi(cad.c_str()));
 		}
	}
 	return estados;
}
 
set<int> cerradura_epsilon_s(string s,string **tafn, int ren, int col){
 	set<int > estados;
 	string edos=s;
 	estados.insert(atoi(s.c_str()));
	stack <string> pila_estados;
	pila_estados.push(s);
 	while(!pila_estados.empty()){
 		string k;
 		k=pila_estados.top();
 		pila_estados.pop();
 		for(int i=0; i<ren; i++){
 			if(tafn[i][0]==k && tafn[i][col-1]!="-"){
 				string temp=tafn[i][col-1];
 				string cad="";
 				char c;
 				for(string::iterator it =temp.begin(); it!=temp.end(); ++it){
 					c=*it;
 					if(*it == ','){
 						edos=edos+","+cad;
 						estados.insert(atoi(cad.c_str()));
 						pila_estados.push(cad);
 						cad="";
 					}
 					else{
 						cad=cad+c;
 					}
 					if(it==temp.end()-1){
 						edos=edos+","+cad;
 						estados.insert(atoi(cad.c_str()));
 						pila_estados.push(cad);
 					}
 				}
 			}
 		}
 	}
 	return estados;
}
 
string mueve(string esta, string t, string **tafn, int ren, int col){
 	set<int> edos = stringToset(esta);
 	set<int> retorno;
	for(set<int>::iterator it=edos.begin(); it!=edos.end(); it++){
 		int nR=0, nC=0;
 		nR=*it;
 		for(int k=1; k<col-1; k++)
 			if(tafn[0][k]==t)
 				nC=k;
		string cad=tafn[nR][nC];
		if(cad!="-"){
			set<int> aux = stringToset(cad);
			for(set<int>::iterator it2=aux.begin(); it2!=aux.end(); it2++){
				retorno.insert(*it2);
			}
		}
 	}
 	return setTostring( retorno );
}
 
bool esfinal( string EdoFinal, int EstadoFinal ){
 	string efi="";
 	efi = static_cast<ostringstream*>(&(ostringstream() << EstadoFinal))->str();
 	string cad="";
 	for(string::iterator it=EdoFinal.begin(); it<EdoFinal.end()+1; it++){
 		if(*it==',' || it==EdoFinal.end() ){
 			if(efi==cad){ return ( true ); }
	 		else{ cad=""; }
 		}
 		else{ cad=cad+*it; }
	}
	 return( false );
}
 
string **afntoafd(string **tafn, string alf, int ren, int col, set<string> *EstadosFinales, int EstadoFinal){
	string **tAfd = NULL;
	map< string, int> nEstados;
	int cont=0;
	string arr[26]={"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
	string s = tafn[1][0];
	string **tt = NULL;
	tt = inicializaMatriz(ren, col-1);	
	int contt = 0;
	for(int i=1; i<ren; i++){ tt[i][0] = arr[contt];  contt++; }
	contt=0;
	for(int i=1; i<col-1; i++){ tt[0][i]=tafn[0][i]; contt++; }
	set<int> edos_alcanzables = cerradura_epsilon_s( s, tafn, ren, col);
	string ed = setTostring(edos_alcanzables);
	nEstados[ed]=cont; 
	cont++;
	char c[10];
	contt=0;
	while(contt<cont){
		for(string::iterator it=alf.begin(); it!=alf.end()-1; it++){
			c[0]=*it;
			string t(c);
			int ncol=0;
			for(int l=1; l<col-1; l++)
				if(tafn[0][l]==t)
					ncol=l;
			string regMueve = mueve( ed, t, tafn, ren, col);
			if(regMueve.length()){
				set<int> EdoF = cerradura_epsilon_s( regMueve, tafn, ren, col );
				string EdoFinal = setTostring(EdoF);
				int ban=0;
				string comp="";
				for(map<string, int>::iterator it2=nEstados.begin(); it2!=nEstados.end(); it2++){
					comp = it2->first;
					int posArr=0;
					if(comp==EdoFinal){
						posArr = it2->second;
						tt[contt+1][ncol]=arr[posArr];
						ban++;
					}
				}
				if(ban==0){
					if( esfinal(EdoFinal, EstadoFinal) ){ EstadosFinales->insert(arr[cont]); }
					nEstados[EdoFinal]=cont;
					tt[contt+1][0]=arr[cont];
					tt[contt+1][ncol]=arr[cont];
					cont++;
				}
			}
		}
		contt++;
		for(map<string, int>::iterator it2=nEstados.begin(); it2!=nEstados.end(); it2++){
			if(contt==it2->second){ ed = it2->first; }
		}
	}
	tAfd=inicializaMatriz(contt+1, col-1);
	for( int i = 0; i <= contt; ++i )
		for( int j = 0; j < col-1; ++j )
			tAfd[i][j]=tt[i][j];
	tAfd[0][0]="E.";
	nRenAFD = contt+1;
	nColAFD = col-1;
	for(int i=1; i<nRenAFD; i++){ tAfd[i][0] = arr[i-1]; }
	ofstream q("Tabla de Transiciones AFD.txt");
	for(int i=0; i<nRenAFD; i++){
		for(int j=0; j<nColAFD; j++){
			q << tAfd[i][j] << "\t" ;
		}
		q <<endl;
	}
	q.close();
	liberaMatriz(&tafn, ren);
	return tAfd; 
}
 
 /*
 *	El la expresi·n regular los "(" -> se representan con "^" (94 ascii)
 *	los ")" -> se representan con "~" (126 en ascii)
 *	La multiplicaci·n esta representada por "·"(170 en ascii)
 */
bool esID(string exp, set<string>abecedario, set<string>numero, string **tablaAFN){
 	int n=exp.size();
 	if(n<2){
 		bool n = abecedario.find(exp) != abecedario.end();
 		if(n){ return true; }
 	}
	for(string::iterator it = exp.begin(); it!=exp.end(); it++){
		
 		char c[2];
		c[0]=*it; c[1]='\0';
 		string a(c);
 		bool abece = abecedario.find(a) != abecedario.end();
 		if(!abece ){
 			bool n = numero.find(a) != numero.end();
 			if(!n){ return false; }
 		}
 	}
 	return true;
}
 
bool esNumero(string exp, set<string>abecedario, set<string>numero, string **tablaAFN){
 	if(isdigit(atoi(exp.c_str()))) return true;
 	int i=1;
 	bool punto=false;
 	while( i<exp.size() ){
 		if( isdigit(exp.at(i)) || exp.at(i) == '.' ){
 			if( exp.at(i)== '.' ){
 				if(punto==false){
 					punto=true;
 				}
 				else{
 					return false;
 				}
 			}
 			i++;
 		}
 	}
 	return true;
}


// Analizador lexico line:586->1084 
void Lexico( string **tablaAFD, set<string> EstadosFinales, string alf, int ren, int col ){
 	set< string > palabras_reservadas;
 	set< string > abecedario;
 	set< string > numero;
 	string arr[52]={"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
 	for( int i=0; i<52; i++ ){ abecedario.insert( arr[i] ); }
 	string arr2[10]={"0","1","2","3","4","5","6","7","8","9"};
 	for( int i=0; i<10; i++ ){ numero.insert( arr2[i] ); }
 	char cadena[100];
 	ifstream d( "PalabrasReservadas.txt" );
 	d.getline( cadena, 100 );
 	while( cadena[0]!='\0' ){
 		string pal( cadena );
 		palabras_reservadas.insert( pal );
		d.getline( cadena, 100 );
 	}
	d.close();
 	string sim, nom;
	map< string, string > Digitos;
	ifstream de( "Digitos.txt" );
 	while( !de.eof() ){
 		de >> sim >> nom ;
 		Digitos[sim]=nom;
 	}
	de.close();
	vector< string >linea;
	vector< string >token;
	vector< string >lexemas;
	vector< string >descripcionE;
	int cont=1, conterr=0, contS=1;
	char cade[100];
	ifstream op("progFuente.cpp");
	while( !op.eof() ){
		op.getline( cade,100 );
		string cad( cade );
		string cad2="";
		char carac[2];
		int renglonTabla=1;
		int columnaTabla=0;
		string posAutomata=tablaAFD[1][0];
		bool finalCadena=false;
		bool puntoNumero=false;
		for( string::iterator it=cad.begin(); it!=cad.end(); it++ ){
			carac[0] = *it;
			carac[1] = '\0';
			char comparat=*it;
			string ac( carac );
			map< string, string >::iterator p = Digitos.find(ac);
			if( p!=Digitos.end() ){
				if( cad2.size()==0 && p!=Digitos.end()){
					lexemas.push_back(ac);
					token.push_back( p->second );
					string ncad = "";
 					ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 					linea.push_back( ncad );
					posAutomata=tablaAFD[1][0];
					cad2="";
				}
				else{
					if( finalCadena && cad2.size()>0 ){
						if(cad2.size()==1){
							bool numeo = numero.find(cad2) != numero.end();
							bool abece = abecedario.find(cad2) != abecedario.end();
							if(numeo){
								token.push_back( "Numero" );
								lexemas.push_back(cad2);
								string ncad = "";
 								ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 								linea.push_back(ncad);
 								cad2 = "";
							}
							else{
								if( abece ){
									token.push_back( "Id" ); 
									lexemas.push_back( cad2 );
									string ncad = "";
 									ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
	 								linea.push_back(ncad);
	 								cad2 = "";
								}
							}
						}
						else{
							finalCadena=false;
							bool esId = esID( cad2, abecedario, numero, tablaAFD );
							if( esId ){ 
								token.push_back( "Id" ); 
								lexemas.push_back(cad2);
								string ncad = "";
 								ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
	 							linea.push_back(ncad);
	 							cad2="";
							}
							else{
								bool esN  = esNumero( cad2, abecedario, numero, tablaAFD );
								if( esN ){
									token.push_back( "Numero" );
									lexemas.push_back(cad2);
									string ncad = "";
 									ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 									linea.push_back(ncad);
 									cad2=="";
								}
								else{
									string::iterator ji=cad2.begin();
									char c=*ji;
									ji=cad2.end()-1;
									char c2=*ji;
									if( c==34 && c2==34){ 
										token.push_back("Literal"); 
										lexemas.push_back(cad2);
										cad2="";
										string ncad = "";
 										ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 										linea.push_back(ncad);
									}
								}
							}
						}
					}
					if(p!=Digitos.end() ){  //Los Digitos en el automata D
						token.push_back( p->second );
						lexemas.push_back(ac);
						string ncad = "";
 						ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 						linea.push_back( ncad );
						posAutomata=tablaAFD[1][0];
						cad2="";
					}
				}
			}
			else{
				//si no es un digito, no hay un " " o un "TAB", entonces se puede concatenar el caracter
				//con la cadena de caracteres anteriores.
				//una vez hecho esto se verifica si es una palabra reservada
				//de no serlo ser·a un numero o Id, sin embargo los numero o id se encuentran al topar
				//un digito ()[]{}*+/; o un " " o un"TAB", y eso se prueba en el el, aqui solo verificamos si es o no una PR
				//o posiblemente el elemento no sea numero ni parte del abecedario y asi se toma como un error y los simbolos en la cadena
				//pasan a ser un id o numero.
				//Aqui es donde cambio la ·posicion del automata y se pone en true o false a finalCadena
				if(ac!=" " || ac!="	"){
					bool numeo = numero.find(ac) != numero.end();
					bool abece = abecedario.find(ac) != abecedario.end();
					if( numeo ){ //en el automata es d(digito del 0-9)
						for( int i=0; i<nRenAFD; i++ )
							if(tablaAFD[i][0]==posAutomata)
								renglonTabla=i;
						for( int i=0; i<nColAFD; i++ )
							if(tablaAFD[0][1]=="d")
								columnaTabla=i;
						posAutomata=tablaAFD[renglonTabla][columnaTabla];
						if(EstadosFinales.find(posAutomata) != EstadosFinales.end())
							finalCadena=true;
						cad2=cad2+ac;
					}
					if( abece || comparat == 34){
						for( int i=0; i<nRenAFD; i++ )
							if(tablaAFD[i][0]==posAutomata)
								renglonTabla=i;
						for( int i=0; i<nColAFD; i++ )
							if(tablaAFD[0][1]=="l")
								columnaTabla=i;
						posAutomata=tablaAFD[renglonTabla][columnaTabla];
						if(EstadosFinales.find(posAutomata) != EstadosFinales.end())
							finalCadena=true;
						cad2=cad2+ac;
						bool palabrasR = palabras_reservadas.find(cad2) != palabras_reservadas.end();
						if( palabrasR && finalCadena ){ 
							token.push_back("Pa.Res."); 
							lexemas.push_back(cad2);
							string ncad = "";
 							ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
	 						linea.push_back(ncad);
	 						cad2="";
						}
						else{
							string::iterator ji=cad2.begin();
							char c=*ji;
							ji=cad2.end();
							char c2=*ji;
							if( c==34 && c2==34){ 
								token.push_back("Literal"); 
								lexemas.push_back(cad2);
								cad2="";
								string ncad = "";
 								ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 								linea.push_back(ncad);
							}
						}
					}
					if(!numeo && !abece){
						if( cad2.size()==0 ){
							if( comparat !=  32  ){
								string ncad = "";
 								ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
								string ca = "Linea " + ncad + " simbolo " + ac + " indefinido.";
								descripcionE.push_back(ca);
								conterr++;
								cad2="";
							}
						}
						else{
							if(cad2.size()==1){
								bool numeo = numero.find(cad2) != numero.end();
								bool abece = abecedario.find(cad2) != abecedario.end();
								if(numeo){
									token.push_back( "Numero" );
									lexemas.push_back(cad2);
									string ncad = "";
 									ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 									linea.push_back(ncad);
 									cad2 = "";
								}
								else{
									if( abece ){
										//cout<<"YO123"<<endl;
										token.push_back( "Id" ); 
										lexemas.push_back( cad2 );
										string ncad = "";
 										ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
	 									linea.push_back(ncad);
	 									cad2 = "";
									}
								}
							}
							else{
								finalCadena=false;
								bool esN  = esNumero( cad2, abecedario, numero, tablaAFD );
								//cout<<"lo de mi numero " << esNumero( cad2, abecedario, numero, tablaAFD )<<endl;
								if( esN ){
									token.push_back( "Numero" );
									lexemas.push_back(cad2);
									string ncad = "";
 									ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 									linea.push_back( ncad );
 									cad2="";
								}
								else{
									bool esId = esID( cad2, abecedario, numero, tablaAFD );
									if( esId ){ 
										//cout<<cad2<<endl;
										token.push_back( "Id" ); 
										lexemas.push_back(cad2);
										string ncad = "";
 										ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
	 									linea.push_back( ncad );
	 									cad2="";
									}	
									else{
										string::iterator ji=cad2.begin();
										char c=*ji;
										ji=cad2.end()-1;
										char c2=*ji;
										if( c==34 && c2==34){ 
											token.push_back("Literal"); 
											lexemas.push_back(cad2);
											cad2="";
											string ncad = "";
 											ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 											linea.push_back( ncad );
										}
									}
								}
							}
						}
					}
				}
				else{
					if( finalCadena && cad2.size()>0 ){
						finalCadena=false;
						bool esId = esID( cad2, abecedario, numero, tablaAFD );
						if( esId ){ 
							token.push_back( "Id" ); 
							lexemas.push_back(cad2);
							string ncad = "";
 							ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
	 						linea.push_back( ncad );
	 						cad2="";
						}
						else{
							bool esN  = esNumero( cad2, abecedario, numero, tablaAFD );
							if( esN ){
								token.push_back( "Numero" );
								lexemas.push_back(cad2);
								string ncad = "";
 								ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 								linea.push_back( ncad );
 								cad2="";
							}
							else{
								string::iterator ji=cad2.begin();
								char c=*ji;
								ji=cad2.end()-1;
								char c2=*ji;
								if( c==34 && c2==34){ 
									token.push_back("Literal"); 
									string ncad = "";
 									ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 									linea.push_back( ncad );
									lexemas.push_back(cad2);
									cad2="";
								}
							}
						}
					}
				}
			}
		}
		if(cad2.size()>0){
			map< string, string >::iterator p = Digitos.find(cad2);
			if( p!=Digitos.end() ){
				lexemas.push_back(cad2);
				token.push_back( p->second );
				string ncad = "";
 				ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 				linea.push_back( ncad );
				posAutomata=tablaAFD[1][0];
				cad2="";
			}
			if( cad2.size()==1 && p==Digitos.end() ){
				bool numeo = numero.find(cad2) != numero.end();
				bool abece = abecedario.find(cad2) != abecedario.end();
				if( numeo ){
					token.push_back( "Numero" );
					lexemas.push_back(cad2);
					string ncad = "";
 					ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 					linea.push_back(ncad);
 					cad2 = "";
				}
				else{
					if( abece ){
						token.push_back( "Id" ); 
						lexemas.push_back( cad2 );
						string ncad = "";
 						ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
	 					linea.push_back( ncad );
	 					cad2 = "";
					}
				}
			}
			else{
				finalCadena=false;
				bool esId = esID( cad2, abecedario, numero, tablaAFD );
				if( esId ){ 
					token.push_back( "Id" ); 
					lexemas.push_back(cad2);
					string ncad = "";
 					ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
	 				linea.push_back( ncad );
	 				cad2="";
				}
				else{
					bool esN  = esNumero( cad2, abecedario, numero, tablaAFD );
					if( esN ){
						token.push_back( "Numero" );
						lexemas.push_back( cad2 );
						string ncad = "";
 						ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 						linea.push_back( ncad );
 						cad2="";
					}
					else{
						string::iterator ji=cad2.begin();
						char c=*ji;
						ji=cad2.end()-1;
						char c2=*ji;
						if( c==34 && c2==34 ){ 
							token.push_back( "Literal" ); 
							lexemas.push_back( cad2 );
							string ncad = "";
 							ncad = static_cast<ostringstream*>(&(ostringstream() << cont))->str();
 							linea.push_back( ncad );
 							cad2="";
						}
					}
				}
			}
		}
		cont++;
	}
	int nR=lexemas.size();
	string **tato = NULL;											//tato <--- tabla de tokens
	tato = inicializaMatriz( nR+1, 3 );
	tato[0][0]="# Linea";tato[0][1]="Token";tato[0][2]="Lexemas";
	int ren2=1; int coluk=0;
	for( vector<string>::iterator it2=linea.begin(); it2!=linea.end(); it2++ ){
			tato[ren2][coluk]=*it2;
		ren2++;
	}
	ren2=1; coluk=1;
	for( vector<string>::iterator it2=token.begin(); it2!=token.end(); it2++ ){
			tato[ren2][coluk]=*it2;
		ren2++;
	}
	ren2=1; coluk=2;
	for( vector<string>::iterator it2=lexemas.begin(); it2!=lexemas.end(); it2++ ){
			tato[ren2][coluk]=*it2;
		ren2++;
	}
	//Vector de simbolos
	vector< string > Simbolos;
	ofstream q( "Tira de tokens.txt" );
	for( int i=0; i<=nR; i++ ){
		for( int j=0; j<3; j++ ){
			q << tato[i][j] << "\t\t" ;
			if(tato[i][1]=="Id")
				Simbolos.push_back( tato[i][2] );
		}
		q <<endl;
	}
	q.close();
	nR = descripcionE.size();
	int contte = 1;
	ofstream te( "Tabla de Errores.txt" );
	te << "#Error" << "\t" << "Descripcion" << endl;
	for (vector<string>::iterator iter = descripcionE.begin(); iter != descripcionE.end(); iter++){
		string cadena = *iter;
		te << contte << "\t" << cadena << endl;
		contte++;
	}
	te.close();
	
	ofstream ts( "Tabla de Simbolos.txt" );
	ts << "#Token" << "\t" << "Id" << endl;
	nR=lexemas.size();
	int contts = 1, i=0;
	vector<string>::iterator palabra_sim = Simbolos.begin();
	while( i<nR+1 ){
		if ( tato[i][1] == "Id" ){
			ts << contts << "\t" << tato[i][2] << endl;
			contts++;
		}
		i++;
	}
	ts.close();
	nR = lexemas.size();
	//Paso los lexemas para ser usados en el Analizador_Semantico
	for(int i=1; i<=nR; i++){
		if(tato[i][1] == "Id"){
			Lexemas_allr.push_back("id");
		}
		else{
			if( tato[i][1] == "Numero" ){
				Lexemas_allr.push_back("Numero");
			}
			else{
				if( tato[i][1] == "Literal" ){
					Lexemas_allr.push_back("Literal");
				}
				else{
					Lexemas_allr.push_back( tato[i][2] );
				}
			}
		}
	}
	Lexemas_allr.push_back("$");
	Lexemas_alnom = lexemas; 
	Lexemas_alnom.push_back("$");
	liberaMatriz( &tato, nR+1 );
}
/* 
 * 
 * 
*/
// comprueba si una produccion tiene un epsilon 
bool tieneEpsilon( map<int, vector<string> > Producciones, string X ){
 	for( map<int, vector<string> >::iterator it2=Producciones.begin(); it2!=Producciones.end(); it2++ ){
 		vector<string>::iterator iter2 = it2->second.begin();
 		int tam=it2->second.size();
 		string B2 = *iter2;
 		if( B2==X ){
 			int cont=2;
 			do{
				string X2 = it2->second[cont];
				if( X2 == "^" ){return true;}
				cont++;
			}while( cont<tam );
		}
 	}
 	return false;
}

// algoritmo para la construccion del conjunto de primeros
void primeros( map<int, vector<string> > Producciones){
 	//Epsilon se toma como el caracter  ^
 	for(set<string>::iterator itera=NoTerminales.begin(); itera!=NoTerminales.end(); itera++){
 		string B = *itera;
 		bool epsilonX=false;
 		for(map<int, vector<string> >::iterator it=Producciones.begin(); it!=Producciones.end(); it++){
 			vector<string>::iterator iter = it->second.begin();
 			if( B == *iter ){
 				string X = it->second[2];
 				bool tern = Terminales.find( X ) != Terminales.end();
 				if( tern  ){ Primeros[B].insert(X); }		//P(X)->Terminal
 				if( !tern  ){ 
				 	epsilonX = tieneEpsilon(Producciones, X); 
					if(epsilonX){Primeros[B].insert("^");} 
				}
 				if( !tern ){
 					set<string>EdosP; set<string>EdosAux;
 					EdosP.insert(X); EdosAux.insert(X);
 					do{
 						bool epsi = false;
 						set<string>::iterator ed=EdosP.begin();
 						string B2 = *ed;
 						EdosP.erase(B2);
 						for(map<int, vector<string> >::iterator it2=Producciones.begin(); it2!=Producciones.end(); it2++){
 							vector<string>::iterator iter2 = it2->second.begin();
	 						if(B2==*iter2){
 								int cont=2;
 								do{
 									string X2 = it2->second[cont];
 									bool terminal = Terminales.find( X2 ) != Terminales.end();
 									if( !terminal && tieneEpsilon(Producciones,X2)){epsi=true;}else{epsi=false; }
 									if(terminal){ Primeros[B].insert(X2); }
	 								else{
	 									bool esta = EdosAux.find( X2 ) != EdosAux.end();
	 									if(!esta){ EdosP.insert( X2 ); EdosAux.insert(X2); }
 									}
 									cont++;
 								}while(epsi);
 							}
	 					}
 					}while( EdosP.size() );
 				}
 			}
 		}
 	}
 	ofstream q( "Primeros.txt" );
 	for( map<string, set<string> >::iterator it3=Primeros.begin(); it3!=Primeros.end(); it3++){
 		q<<"P( "<<it3->first<<" ) { ";
 		for( set<string>::iterator iter3 = it3->second.begin(); iter3 != it3->second.end(); iter3++){ q<<*iter3<<" "; }
 		q<<" }"<<endl;
 	}
 	q.close();
}

void siguientes( map<int, vector<string> > Producciones ){
 	for(set<string>::iterator itera = NoTerminales.begin(); itera!=NoTerminales.end(); itera++){
 		string SnnT = *itera;//Simbolo no terminal--> SnnT
 		set<string>EdosP; set<string>EdosAux;
 		EdosP.insert(SnnT); EdosAux.insert(SnnT);
 		do{
 			bool porUno=false;
 			set<string>::iterator SnT = EdosP.begin(); EdosP.erase(SnT);
 			for(map<int, vector<string> >::iterator it=Producciones.begin(); it!=Producciones.end(); it++){
 				vector<string> Prod = it->second;
 				vector<string>::iterator g = Prod.begin(); //Prod.erase(g);
 				string A="", beta="", B="";
	 			for(int i=1; i<Prod.size(); i++){
 					if( Prod.at(i)== *itera ){
 						porUno=true;
 						if( Prod.size()-i > 1 ){
 							B=Prod.at(0); A=Prod.at(i); beta=Prod.at(i+1);
 							bool terminal = Terminales.find( beta ) != Terminales.end();
 							//Poner $ en S, donde S es el simbolo inicial
 							if( it == Producciones.begin() ){
 								Siguientes[B].insert( "$" );
 							}
		 					if( terminal ){ Siguientes[A].insert( beta ); }
 							else{
 								if( beta != B ){
 									bool terminalbeta = Primeros[beta].find( "^" ) != Primeros[beta].end();
 									//Todo lo que esta en P(beta) excepto epsilon ponerlo en siguientes de A
 									for( map<string, set<string> >::iterator it3=Primeros.begin(); it3!=Primeros.end(); it3++){
 										if( it3->first == beta)
 											for( set<string>::iterator set_iter = it3->second.begin(); set_iter != it3->second.end(); set_iter++){ 
 												Siguientes[A].insert( *set_iter ); 
											}
			 						}
 									if( terminalbeta ){
 										//Si tenia epsilon se borra ya que no debe de tenerlo en siguientes por la regla 2 del algoritmo
										Siguientes[A].erase("^");
										//Busco 
										bool besta = EdosAux.find(B)!=EdosAux.end();
 										if( !besta ){ 
 											if( B!=*itera ){
										 		EdosAux.insert(B); EdosP.insert(B);
											}
										}
										for( map<string, set<string> >::iterator it4=Siguientes.begin(); it4!=Siguientes.end(); it4++){
 											if( it4->first==B ){
 												for( set<string>::iterator iter4 = it4->second.begin(); iter4 != it4->second.end(); iter4++){
	 												Siguientes[A].insert(*iter4);
 												}
 											}
 										}
									}
 								}
 							}
 						}
		 				else{
 							B=Prod.at(0); A=Prod.at(i);
 							bool besta = EdosAux.find(B)!=EdosAux.end();
 							if( !besta ){ EdosAux.insert(B); EdosP.insert(B); }
	 					}
 					}
 				}
 			}
 			if( porUno == false ){ 
			 	Siguientes[*itera].insert("$"); 
			}
 		}while( EdosP.size() );
 		EdosP.clear(); EdosAux.clear();
 	}
 	for( map<string, set<string> >::iterator itera=Siguientes.begin(); itera!=Siguientes.end(); itera++ ){
 		for( map<int, vector<string> >::iterator it=Producciones.begin(); it!=Producciones.end(); it++ ){
 			vector<string> Prod = it->second;
 			string A="", beta="", B="";
	 		for(int i=1; i<Prod.size(); i++){
	 			if( Prod.at(i)== itera->first ){
 					if( Prod.size()-i == 1 ){
 						B=Prod.at(0); A=Prod.at(i);
 						if(A!=B){
 							for( map<string, set<string> >::iterator it4=Siguientes.begin(); it4!=Siguientes.end(); it4++){
 								if(it4->first==B){
 									for( set<string>::iterator iter4 = it4->second.begin(); iter4 != it4->second.end(); iter4++){
 										Siguientes[A].insert(*iter4);
 									}
 								}
 							}
 						}
	 				}
 				}
	 		}
 		}
 	}
	ofstream q( "Siguientes.txt" );
	for(set<string>::iterator itera=NoTerminales.begin(); itera!=NoTerminales.end(); itera++){
 		string SnnT = *itera;
		bool esta = Siguientes.find(*itera)!=Siguientes.end();
		Siguientes[*itera].insert("");
		if(!esta){ 
			bool mr=false;
			for(map<int, vector<string> >::iterator it=Producciones.begin(); it!=Producciones.end(); it++){
 				vector<string> Prod = it->second;
 				for(int i=1; i<Prod.size(); i++){
 					if(*itera==Prod.at(i)){ mr=true; }
 				}
			} 
			if( !mr ){
				Siguientes[*itera].insert("$");
				siguientes( Producciones ); 
			}
			else{ 
				siguientes( Producciones );
			}
		}
	}
 	for( map<string, set<string> >::iterator it4=Siguientes.begin(); it4!=Siguientes.end(); it4++){
 		q<<"S( "<<it4->first<<" ) { ";
 		for( set<string>::iterator iter4 = it4->second.begin(); iter4 != it4->second.end(); iter4++){ q<<*iter4<<" "; }
 		q<<" }"<<endl;
 	}
 	q.close();
}
 
void verificadorSiguientes( map<int, vector<string> > Producciones  ){
 	for( map<string, set<string> >::iterator itera=Siguientes.begin(); itera!=Siguientes.end(); itera++ ){
 		for( map<int, vector<string> >::iterator it=Producciones.begin(); it!=Producciones.end(); it++ ){
 			vector<string> Prod = it->second;
 			string A="", beta="", B="";
	 		for(int i=1; i<Prod.size(); i++){
	 			if( Prod.at(i)== itera->first ){
 					if( Prod.size()-i == 1 ){
 						B=Prod.at(0); A=Prod.at(i);
 						if(A!=B){
 							for( map<string, set<string> >::iterator it4=Siguientes.begin(); it4!=Siguientes.end(); it4++){
 								if(it4->first==B){
 									for( set<string>::iterator iter4 = it4->second.begin(); iter4 != it4->second.end(); iter4++){
 										Siguientes[A].insert(*iter4);
 									}
 								}
 							}
 						}
	 				}
 				}
	 		}
 		}
 	}
 	ofstream q( "Siguientes.txt" );
 	for( map<string, set<string> >::iterator it4=Siguientes.begin(); it4!=Siguientes.end(); it4++){
 		q<<"S( "<<it4->first<<" ) { ";
 		for( set<string>::iterator iter4 = it4->second.begin(); iter4 != it4->second.end(); iter4++){ q<<*iter4<<" "; }
 		q<<" }"<<endl;
 	}
 	q.close();
}
 
void primeros_siguientes( ){
 	map<int, vector<string> > Producciones;
 	string prod="";
 	char cade[100];
	string palabra;
	ifstream de("Producciones.txt");
	int cont=1;
	while( !de.eof() ){
 		de >> palabra ;
 		Producciones[cont].push_back( palabra );
 		NoTerminales.insert( palabra );
 		de.getline( cade, 100 );
 		string cadena(cade);
 		char carac[2];
 		string aux="";
 		for( string::iterator it=cadena.begin(); it!=cadena.end()+1; it++ ){
			carac[0] = *it;
			carac[1] = '\0';
			string ac( carac );
			if(ac!=" "){ aux=aux+ac; }
			else{
				if(aux!="->")
					Producciones[cont].push_back(aux);
				aux="";
			}
		}
		if(aux.size()!=0 && aux!=" "){ Producciones[cont].push_back(aux); }
 		cont++;
 	}
 	de.close();
 	ifstream po("Producciones.txt");
	while( !po.eof() ){
 		po >> palabra ;
 		bool tern = NoTerminales.find( palabra ) != NoTerminales.end();
		if( !tern && palabra!="->" ){ Terminales.insert( palabra ); }
 	}
 	po.close();
 	for(map<int, vector<string> >::iterator it=Producciones.begin(); it!=Producciones.end(); it++){
		vector<string> Prod = it->second;
		for(int i=0; i<Prod.size(); i++){
			cout<<Prod.at(i)<<" ";
		}
		cout<<endl;
	}
 	primeros( Producciones );
 	siguientes( Producciones );
 	verificadorSiguientes( Producciones );
 	verificadorSiguientes( Producciones );
}
bool esta_cc(vector< vector<string> > c, vector<string> it3){
 	int n=c.size();
 	for(int i=0; i<n; i++){
 		if(c[i]==it3){ return true; }
 	}
 	return false;
}

/**
 * 
 * 
 * Construcción de la tabla de análisis sintáctico
 *  
 * 
*/

//   · -> 250 ( valor en ascii )
vector< vector<string> > cerradura_cc( vector< vector<string> > estados, map<int, vector<string> > Producciones2){
 	map<int, vector<string> > ProduccionesAum;
 	ProduccionesAum = Producciones2;
 	vector<string>Jota;
 	for(map<int, vector<string> >::iterator it = ProduccionesAum.begin(); it!=ProduccionesAum.end(); it++){
 		vector<string> it2 = it->second;
 		Jota.push_back(it2.at(0));
 		Jota.push_back("·");
 		for(int i=2; i<it->second.size(); i++){ 
 			if(it2.at(i)!="^"){
 				Jota.push_back(it2.at(i)); 
 			}
		}
 		it->second = Jota; 
		Jota.clear();
 	}
 	Jota.clear();
	//
	vector< vector<string> > c;
 	queue< vector<string> > Aux2;
 	int contEn=0, n=estados.size(), contE=0;
	for(int i=0; i<n; i++ ){ c.push_back( estados[i] ); Aux2.push(estados[i]); }
	set<string> verifica; verifica.insert("");
	contEn=n;
	while( !Aux2.empty() ){
		Jota = Aux2.front(); Aux2.pop();
		bool point=false;
		int contV=0;
		for(vector<string>::iterator it2=Jota.begin(); it2!=Jota.end(); it2++, contV++){
			int n=Jota.size();
			if( *it2 == "·" && ((it2+1) != Jota.end() ) ){
				string B;
				vector<string>::iterator it3 = it2;
				it3++;
				B = *it3;
				bool esta =verifica.find( B )!=verifica.end();
				if( !esta )
					if( NoTerminales.find( B ) != NoTerminales.end() ){
						int contPA=0;
						for(map<int, vector<string> >::iterator iti=ProduccionesAum.begin(); iti!=ProduccionesAum.end(); iti++, contPA++){
							vector< string > it3 = iti->second;
							if( esta_cc(c, it3)==false && iti->second.at(0)==B ){
								if( iti->second.size()==2 && iti->second.at(1)=="^" ){
									vector<string> epsilon; 
									epsilon.push_back(iti->second.at(0));
									epsilon.push_back("·");
									if(!esta_cc(c, epsilon)){
										verifica.insert( B );
 										c.push_back( epsilon );
 										Aux2.push( epsilon );
 										contEn++;
									}
								}
								else{
									verifica.insert(B);
 									c.push_back(it3);
 									Aux2.push(it3);
 									contEn++;
								}
 							}
 						}
					}
					else{
						for(map<int, vector<string> >::iterator iti=ProduccionesAum.begin(); iti!=ProduccionesAum.end(); iti++){
							vector< string > it3 = iti->second;
							if( esta_cc(c, it3)==false && iti->second.at(0)==B ){
								verifica.insert(B);
 								c.push_back(it3);
 								Aux2.push(it3);
 								contEn++;
							}
 						}
					}
			}
		}
		contE++;
	}
	return c;
}
 
vector< vector<string> > ir_a_cc(vector< vector<string> > estados, string y){
 	vector< vector<string> > ira;
 	string x=y;
 	for(int j=0; j<estados.size(); j++){
 		vector<string> edo = estados.at(j);
 		for(int i = 0; i < edo.size(); i++){
 			if( edo.at(i)==x ){
 				if(i>0){
 					if(edo.at(i-1)=="·"){
 						vector<string> n = estados.at(j);
	 					vector<string> nn;
 						for(int k = 0; k < n.size(); k++){
 							if( k+1==i ){
 								nn.push_back(edo.at(i));
 								nn.push_back("·");
 								k++;
 							}
 							else{
 								nn.push_back(n.at(k));
 							}
 						}
 						if( !esta_cc(ira, nn) ){
 							ira.push_back(nn);
 						}
 					}
 					break;
 				}
 			}
 		}
 	}
	return ira;
}
 
int esta_vec(queue< vector< vector<string> > > Estados, vector< vector<string> > c ){
 	int cont=0;
 	while(!Estados.empty()){
 		vector< vector<string> > Jota;
 		Jota = Estados.front(); Estados.pop();
 		if(Jota == c){
 			return cont;
 		}
 		cont++;
 	}
 	return -1;
}
 
int numPro(vector<string> res,map<int, vector<string> > ProduccionesAum){
 	int cont=0;
 	for(map<int, vector<string> >::iterator it=ProduccionesAum.begin(); it!=ProduccionesAum.end(); it++, cont++){
 		if(it->second==res){
 			return cont;
 		}
 	}
 	return 10000;
}


void tablaAnalisis(queue< vector <vector<string> > >Estados, map<int, vector<string> > Producciones2 ){
 	/***
	  * Inserta $ y quite el simbolo E(^) de los terminales
	 */
 	bool ter_content = Terminales.find("$")!=Terminales.end();
 	if(!ter_content){
		Terminales.insert("$");
	}
 	bool have_eps = Terminales.find("^")!=Terminales.end();
 	if( have_eps ){
 		Terminales.erase("^");
 	}
 	int col = Terminales.size()+NoTerminales.size()+1;
 	int ren = Estados.size()+1;
 	//variables que se ocupan en el analizador sintactico lr
 	ren_tas=ren; col_tas=col;
 	//string **tabla_analisis = NULL;		hago la tabla global para que sea utilizada por la funcion analizador sintactico lr
	tabla_analisis = inicializaMatriz( ren, col );
 	tabla_analisis[0][0]="Edos.";
 	int cont=1;
 	map< string, int >Ncolum;
 	for( set<string>::iterator it=Terminales.begin(); it!=Terminales.end(); it++, cont++){ 
		if(*it!="^"){ 
			tabla_analisis[0][cont]=*it; 
			Ncolum[*it]=cont; 
		}
	}
	if( have_eps ){
 		Terminales.insert("^");
 	}
	Terminales.erase("$");
 	for( set<string>::iterator it=NoTerminales.begin(); it!=NoTerminales.end(); it++, cont++){ 
		tabla_analisis[0][cont]=*it; 
		Ncolum[*it]=cont; 
	}
 	//Obtengo las producciones aumentadas
	map<int, vector<string> > ProduccionesAum;
 	ProduccionesAum = Producciones2;
 	vector<string>Jota;
 	for( map<int, vector<string> >::iterator it = ProduccionesAum.begin(); it!=ProduccionesAum.end(); it++ ){
 		vector<string> it2 = it->second;
 		Jota.push_back(it2.at(0));
 		for( int i=2; i<it->second.size(); i++ ){ 
 			if(it2.at(i)!="^"){ Jota.push_back(it2.at(i));  }
		}
		Jota.push_back("·");
 		it->second = Jota; Jota.clear();
 	}
 	vector<string>primer = ProduccionesAum[0];
 	cont=1;
 	queue< vector <vector<string> > >Aux = Estados;
 	while( !Aux.empty() ){
 		tabla_analisis[cont][0] = "I"+ static_cast<ostringstream*>(&(ostringstream() << cont-1))->str();
 		vector <vector<string> > Aux1 = Aux.front(); Aux.pop();
 		for( vector <vector<string> >::iterator it=Aux1.begin(); it!=Aux1.end(); it++ ){
 			vector<string> pro = *it;
 			int tam = pro.size();
 			for( int i=0; i<tam; i++ ){
 				if( pro.at(i)=="·" ){
 					if( i+1==tam ){
 						int n= numPro( pro, ProduccionesAum );
 						if( n!=1000 ){
							string ncad = "r";
 							ncad = ncad+static_cast<ostringstream*>(&(ostringstream() << n))->str();
 							set<string>columi=Siguientes[pro.at(0)];
 							for( set<string>::iterator it= columi.begin(); it!=columi.end(); it++){
 								int nco = Ncolum[*it];
 								if(nco!=0){
 									if(tabla_analisis[cont][nco]=="-"){
		 								tabla_analisis[cont][nco] = ncad;
 									}
								}
 							}
 						}
 					}
 					else{
 						string A = pro.at(i+1);
 						int n= numPro( pro, ProduccionesAum );
 						bool es_ter = Terminales.find( A ) != Terminales.end();
 						if( es_ter ){
 							vector< vector<string> > Jota = cerradura_cc( ir_a_cc( Aux1, A ) , Producciones2 ) ;
 							int e_v = esta_vec( Estados, Jota );
 							if( e_v != -1 && Jota.size()>0 ){
 								string ncad = "d";
 								ncad = ncad + static_cast<ostringstream*>(&(ostringstream() << e_v))->str();
 								int nco = Ncolum[A];
 								if( tabla_analisis[cont][nco]=="-" ){
	 								tabla_analisis[cont][nco] = ncad;
 								}
 							}
 							else{
 								int nco = Ncolum[A];
	 							tabla_analisis[cont][nco] = "Error";
 							}
 						}
 						else{
 							if(A=="$"){
 								int nco = Ncolum[A];
	 							tabla_analisis[cont][nco] = "Acep.";
 							}
 							else{
 								bool es_nter = NoTerminales.find( A ) != NoTerminales.end();
 								if( es_nter ){
 									vector< vector<string> > Jota = cerradura_cc( ir_a_cc( Aux1, A ) , Producciones2 ) ;
 									int e_v = esta_vec( Estados, Jota );
 									if( e_v!=-1 ){
 										string ncad = "";
 										ncad = ncad + static_cast<ostringstream*>(&(ostringstream() << e_v))->str();
 										int nco = Ncolum[A];
 										if( tabla_analisis[cont][nco]=="-" ){
	 										tabla_analisis[cont][nco] = ncad;
 										}
 									}
 								}
 							}
 						}
 					}
 				}
 			}
 		}
 		cont++;
 	}
 	ofstream q( "Tabla_de_Analisis_Sintactico.txt" );
 	for(int i=0; i<ren_tas; i++){
 		for(int j=0; j<col_tas; j++){
 			q<<tabla_analisis[i][j]<<"	";
 		}
 		q<<endl;
 	}
 	q.close();
}
 
string accion_tas( int s, string a ){//tas-> tabla de analisis sintactico
 	int nE_nTr = s+1;
 	for(int i=0; i<col_tas; i++){
 		if( tabla_analisis[0][i] == a ){
 			return tabla_analisis[nE_nTr][i];
 		}
 	}
}
 
string traduccion( int n, string t, vector<string>pila ){
 	vector<string> atributos = reglas[n];
 	int na = atributos.size();
 	string concatenacion = "";
 	int i=2;
 	bool seguir=true;
 	while( i<na && seguir ){
 		if(atributos[i].at(0)==34){
 			char cadena[atributos[i].size()];
 			int j=1;
 			while(atributos[i].at(j)!='"'){
 				cadena[j-1]=atributos[i].at(j);
 				j++;
 			}
 			string cad = cadena;
			cout << cadena << "   ,   j: " << j << endl;
 			string cad2="";
 			for(int i=0; i<j-1; i++){
				if(cad.at(i)!=34 && cad.at(i)!=39  ){
					cad2 = cad2  + cad.at(i);
				}
 			}
 			concatenacion = concatenacion + cad2 ;
 		}
 		else{
 			//atributo tok
 			string tok = atributos[i];
 			string par = "";
 			int j;
 			for( j=0; j<tok.size(); j++){
 				if(tok.at(j)!='.'){
 					par = par + tok.at(j);
 				}
 				else{
 					break;
 				}
 			}
 			for( int k=0; k < pila.size(); k++){
 				string b = pila[k].substr(0,j);
 				if( b == par ){
 					int n2 = pila[k].size();n2--;n2--;
 					string c = pila[k].substr(j+2,n2);
 					string cad2="";
 					for(int i=0; i<c.size(); i++){
		 				if(c.at(i)!=34 && c.at(i)!=39 )
 							cad2 = cad2 + " " + c.at(i);
 					}
 					concatenacion = concatenacion + cad2  ;
 				}
 				if(b == par)
 					break;
 			}
 		}
 		i++;
 		if( atributos[i] == "||" ){
 			seguir=true;
 			i++;
 		}
 		else{
 			return concatenacion;
 		}
 	}
 	return concatenacion;
}
 
void analizador_sintactico_LR( queue< vector <vector<string> > >Estados, map<int, vector<string> > Producciones ){
	string **tabla = NULL;
	tabla = inicializaMatriz( ren_tas, col_tas );
	for(int i=0; i<ren_tas; i++){
		for(int j=0; j<col_tas; j++){
			tabla[i][j]=tabla_analisis[i][j];
		}
	}
 	/*En Lexemas_allr se encuentra la tira de tokens
 		vector< string >Tira_tokens = Lexemas_allr;
	 Sin embargo en nuestro caso usaremos una prueba que sera: float id , id ; $	
	*/
 	vector< string >Tira_tokens = Lexemas_allr;
 	//hago una copia de los lexemas_alnom 
 	vector< string >Tira_nom = Lexemas_alnom;
 	//vector< string >Tira_tokens;
 	//Tira_tokens.push_back("$");
 	//Tira_tokens.push_back("float");Tira_tokens.push_back("id");Tira_tokens.push_back(",");
	//Tira_tokens.push_back("id");Tira_tokens.push_back(";");Tira_tokens.push_back("$");
	
 	stack< string >mPila;
 	stack< string >mPilaT;
 	int contenidos = 0;
 	
 	mPila.push("0");
 	mPilaT.push("0");
 	
	string s, a;
	//tira_n es una variable a la posicion actual de la tira de tokens
 	int tira_n=0;
 	ofstream q( "Analisis_Sintactico.txt" );
 	q<<"Pila\t\t\t\t\t"<<"Tira_Tokens\t\t\t\t\t"<<"Salida"<<endl;
 	bool seguir = true;
 	bool es_s =false;
 	int r, c;
 	int solo1=0;
 	do{
 		//s -> es el apuntador a la pila
 		s = mPila.top();
 		//a -> es el apuntador a la tira de tokens
 		a = Tira_tokens.at( tira_n );
 		string salida = accion_tas( atoi( s.c_str() ), a );
 		stack< string >Aux = mPila;
 		stack< string >AuxT = mPilaT;
 		int n_p=mPila.size();
 		while( n_p-- ){
 			q<<AuxT.top()<<" "; AuxT.pop();
 		}
 		q<<"\t\t\t\t\t";
 		for(int i=tira_n; i<Tira_tokens.size(); i++){
 			if( Tira_tokens.at(i)=="id" || Tira_tokens.at(i)=="Numero" || Tira_tokens.at(i)=="Literal" ){
 				q<<Tira_tokens.at(i)<<".'"<<Lexemas_alnom.at(i)<<"'";
 			}
 			else{
 				q<<Tira_tokens.at(i)<<" ";
 			}
 		}
 		q<<"\t\t\t\t\t";
 		switch( salida.at(0) ) { 
   			case 'd':{
   					int cont=1;
   					string num_sal="";
   					while( cont<salida.size() ){
   						num_sal = num_sal + salida.at(cont);
   						cont++;
   					}
   					if( a=="id" || a=="Numero" || a=="Literal" ){
   						string auxiliarT = Tira_nom.at( tira_n );
   						mPila.push( a );
   						string auxiliarB = a+".'"+auxiliarT+"'";
   						mPilaT.push( auxiliarB );
   					}
   					else{
   						mPila.push( a );
   						mPilaT.push( a );
   					}
	   				mPila.push( num_sal );
	   				mPilaT.push( num_sal );
   					q<<salida<<endl;
  					s = mPila.top();
  					if(a!="$"){
  						tira_n++;
  					}
   					a = Tira_tokens.at( tira_n );
   					break;
   				}
   			case 'r':{
					//cout<<"reduccion"<<endl;
   					//emitir la produccion A·-> B
   					string num_sal = "";
   					int n;
   					int cont=1;
   					while( cont<salida.size() ){
   						num_sal = num_sal + salida.at(cont);
   						cont++;
   					}
   					n = atoi( num_sal.c_str() );
   					q<<salida<<" ";
					for(int i=0; i<Producciones[n].size(); i++){
						if(i!=1)
   							q<<Producciones[n].at(i)<<" ";
   						else
	   						q<<"->"<<Producciones[n].at(i)<<" ";
   					}
   					//Si es una reducciones debe de llevar su correspondiente regla semantica
   					//string concatenacion = "";
   					for(int i=0; i<reglas[n].size(); i++){
   						q<<reglas[n].at(i)<<" ";
   					}
   					map<int, string>TradsRed;
   					//pop 2*|B| simbolos
   					int m=0;
   					if(solo1==0){
   						if( Producciones[n].at(Producciones[n].size()-1) != "^" ){
   							m=Producciones[n].size()-2; m*=2; solo1++;
   						}
   					}
   					else{
   						if( Producciones[n].at(Producciones[n].size()-1) != "^" ){
   							m=Producciones[n].size()-2; m*=2;
   						}
   					}
   					vector<string> pila2;
   					for (int i=0; i<m; i++){
   						if(!isdigit(atoi(mPilaT.top().c_str())))
   							pila2.push_back(mPilaT.top());
   						mPila.pop();
   						mPilaT.pop();
   					}
   					//Para sacar el string
   					string trad = "";
   					if(reglas[n].size()==4){
   						trad = reglas[n].at(2);
   					}
   					else{
   						trad = traduccion( n, Producciones[n].at(0), pila2);
   					}
   					//cout<<"Trad:= "<<trad<<endl;
   					//cout<<"\n tama·o de pila = "<<mPila.size()<<endl;
					//j simbolo en el tope de la pila
					string muest = mPila.top();
   					int jota = atoi( muest.c_str() );
					//push A
					string	 cadenaTrans = Producciones[n].at(0);
					mPila.push( cadenaTrans );
					char c=34;
   					cadenaTrans = cadenaTrans + "."+ c + trad + c;
					mPilaT.push( cadenaTrans );
   					//s=Ir_a[j,A]		sacar el estado j, con el numero jota
   					queue< vector<vector<string> > >aux2=Estados;
   					vector<vector<string> >estado1;
   					int cont_2=0;
   					while( !aux2.empty()){
						if( cont_2==jota ){
 							estado1 = aux2.front();
 							break;
   						}
   						else{ aux2.pop(); }
   						cont_2++;
					}
   					vector< vector<string> >edo = cerradura_cc( ir_a_cc( estado1, Producciones[n].at(0) ),  Producciones );
   					//push s
   					int es=0;
   					string nc="";
					if(edo.size()>0){
						es = esta_vec(Estados, edo);
   						nc = accion_tas( atoi( s.c_str() ), Producciones[n].at(0) );
   					}
   					if(es>0){
   						bool es_nter = NoTerminales.find( Producciones[n].at(0) ) != NoTerminales.end();
   						if(es_nter){
   							if(es == atoi(nc.c_str()) ||  nc=="-"){
   								string ncad = "";
 								ncad = ncad + static_cast<ostringstream*>(&(ostringstream() << es))->str();
   								mPila.push( ncad );
   								mPilaT.push( ncad );
   							}
   							else{
   								mPilaT.push( nc );
   								mPila.push( nc );
   							}
   						}
   						else{
   							string ncad = "";
 							ncad = ncad + static_cast<ostringstream*>(&(ostringstream() << es))->str();
   							mPila.push( ncad );
   							mPilaT.push( ncad );
   						}
	   				}
	   				else{
	   					//cout<<" svalue =  "<<atoi( s.c_str())<< endl;
	   					string nc = accion_tas( atoi( s.c_str() ), Producciones[n].at(0) );
	   					if(nc=="-"){
	   						int ntt=1;
	   						bool es_n_ter = NoTerminales.find( Producciones[n].at(0) ) != NoTerminales.end();
	   						if( es_n_ter ){
	   							ntt = ntt + Terminales.size();
	   							for( set<string>::iterator it=NoTerminales.begin(); it!=NoTerminales.end(); it++, ntt++)
 									if( Producciones[n].at(0) == *it )
										nc = tabla[atoi(s.c_str())][ntt];
	   						}
	   						else{
	   							for( set<string>::iterator it=Terminales.begin(); it!=Terminales.end(); it++, ntt++)
 									if( Producciones[n].at(0) == *it ){
 										//cout<<Producciones[n].at(0) << *it<<endl;
 										nc = tabla[atoi(s.c_str())][ntt];
 									}
	   						}
	   						mPilaT.push( nc );
	   						mPila.push(nc);
	   					}
	   					else{
	   						mPila.push( nc );
	   						mPilaT.push( nc );
	   					}
	   				}
   					q<<endl;
   					break;
   				}
   			case 'A': {
				q<<"Aceptacion"<<endl;
   				seguir = false;
   				break;
   			}
   			case '-': {
   				seguir = false;
   				break;
   			}
   			default:{	//cout<<"No se encontro nada en la tabla";
   				seguir = false;
   			}
		}
 	}while( seguir );
 	q.close();
} 
 
void obtenerReglas(){
 	//map<int, vector<string> >reglas;---->global
 	char cade[100];
	ifstream archivo("Reglas_Semanticas.txt");
	int cont=1;
	while( !archivo.eof() ){
 		archivo.getline( cade, 100 );
 		string cadena(cade);
 		char carac[2];
 		string aux="";
 		for( string::iterator it=cadena.begin(); it!=cadena.end()+1; it++ ){
			carac[0] = *it;
			carac[1] = '\0';
			string ac( carac );
			if(ac!=" "){ aux=aux+ac; }
			else{
				reglas[cont].push_back(aux); 
				aux="";
			}
		}
		if(aux.size()!=0 && aux!=" "){ reglas[cont].push_back(aux); }
 		cont++;
 	}
 	archivo.close();
}
 
void coleccion_canonica(){
	//Se obtienen las producciones aumentadas
	map<int, vector<string> > Producciones2;
 	string prod="";
 	char cade[100];
	string palabra;
	ifstream de("Producciones.txt");
	int cont=1;
	while( !de.eof() ){
 		de >> palabra ;
 		Producciones2[cont].push_back( palabra );
 		de.getline( cade, 100 );
 		string cadena(cade);
 		char carac[2];
 		string aux="";
 		for( string::iterator it=cadena.begin(); it!=cadena.end()+1; it++ ){
			carac[0] = *it;
			carac[1] = '\0';
			string ac( carac );
			if(ac!=" "){ aux=aux+ac; }
			else{
				if(aux!="->"){ Producciones2[cont].push_back(aux); }
				aux="";
			}
		}
		if(aux.size()!=0 && aux!=" "){ Producciones2[cont].push_back(aux); }
 		cont++;
 	}
 	de.close();
 	cont=0;
 	map<int, vector<string> >::iterator it=Producciones2.begin();
 	vector<string> Primero;
 	vector<string> Prod = it->second;
 	string cad = Prod.at(0);
 	cad=cad+"'";
 	bool esta=false;
	for(map<int, vector<string> >::iterator it=Producciones2.begin(); it!=Producciones2.end(); it++){
 		vector<string>::iterator it2 = it->second.begin();
 		for(int i=0; i < it->second.size(); i++){ 
			if(it2[i]==cad){ 
				esta=true;
			}
		}
 	}
 	if(esta){
 		cad=cad+"'";
 		Producciones2[cont].push_back(cad);
 		Primero.push_back(cad);
 	}
 	else{ 
		Producciones2[cont].push_back(cad);
		Primero.push_back(cad); 
	}
 	Primero.push_back("·");Primero.push_back(Prod.at(0));Primero.push_back("$");
	//
 	Producciones2[cont].push_back(Prod.at(0));
 	Producciones2[cont].push_back("$");
 	vector< vector<string> > Aux;
 	Aux.push_back( Primero );
	vector< vector<string> > Jota = cerradura_cc(Aux,Producciones2);
	//vector< vector<string> > Jota = cerradura_cc( ir_a_cc( Aux, Prod.at(0) ) , Producciones2 );
	ofstream q( "Coleccion_Canonica.txt" );
 	queue< vector< vector<string> > > Estados;
	queue< vector< vector<string> > > Aux2;
	Estados.push(Jota); Aux2.push(Jota);
	int contE=0;
 	cont=0;
 	q<<"I"<<contE<<"=({";
 	for(int j=0; j<Jota.size(); j++){
 		vector<string> edo = Jota.at(j);
 		q<<" [ ";
 		for(int i = 0; i < edo.size(); i++)
     	  	if(i==0){q<<edo.at(i)<<" -> "; }
 			else{ q<<edo.at(i)<<" "; } 
 		q<<" ] ";
 	}
	q<<")}\n";
	contE++;
	Jota.clear();
	//cont es el numero de estado actual, contE es el numero de estado creados (el limite de estados)
	while( cont<contE ){
 		Jota = Aux2.front(); Aux2.pop();//saco el estado al frente de la pila auxiliar
		vector<string>Ir; set<string>Iraw;
		for(int j=0; j<Jota.size(); j++){
 			vector<string> edo = Jota.at(j);
			for(int i = 0; i < edo.size()-1; i++)
				if(edo.at(i)=="·"){
					if((i+1)!=edo.size()){
						string cad=edo.at(i+1);
			 			if( !(Iraw.find(cad)!=Iraw.end()) ){
							Ir.push_back(cad);
							Iraw.insert(cad);
						}
					}
				}
		}
		Iraw.clear();
		for(vector<string>::iterator it=Ir.begin(); it!=Ir.end(); it++){
			if(*it!="$"){
				vector< vector<string> > Jota2 = cerradura_cc( ir_a_cc( Jota, *it ) , Producciones2 );
				int es = esta_vec(Estados, Jota2);
				if( es < 0 ){
					Estados.push(Jota2); Aux2.push(Jota2);
					q<<"I"<<contE<<"=({";
 					for(int m=0; m<Jota2.size(); m++){
 						vector<string> edo;
 						if(m!=Jota2.size()){
 							edo = Jota2.at(m);
		 					q<<" [ ";
 							for(int k = 0; k < edo.size(); k++){
 								if(k==0){q << edo.at(k)<<" -> "; }
 								else{ q << edo.at(k)<<" "; } 
 							}
	 						q<<" ] ";
 						}
 					}
					q<<")}\n";
					contE++;
				}
			}
			else{ q<<"Ir_a(I"<<cont<<",$)=Aceptacion\n"; }
		}
 		cont++;
 		Jota.clear();
 	}
 	q.close();
 	//Tabla analisis
	tablaAnalisis( Estados, Producciones2 );
	obtenerReglas();
	analizador_sintactico_LR( Estados, Producciones2 );
 }
 
int main() {
	set<char> simbolos;	
	char cadena[100];
	string **tablaAFN = NULL;
	//obtenemos la expresi·n regular que genera el lenguaje a traducir
	ifstream d("er.txt"); 
	d.getline(cadena, 100);
	d.close();
	string er(cadena);
	//Obtenemos el aut·mota finito no definido
	AFN required_afn;
	required_afn = er_a_afn(er);
	//obtenemos el alfabeto del lenguaje
	string alf=ObtenerAlfabeto(er);
	int ren, col;
	ren = required_afn.get_contar_vertices()+1;
	col = alf.length()+1;
	//Obtener el edo final del 
	int EstadoFinal = required_afn.get_estado_final()+1;
	tablaAFN = getTable(required_afn, er, alf, ren, col);
	//AFN-AFD la tranformaci·n del aut·mata a determinista
	string **tablaAFD = NULL;
	set<string>EstadosFinales, *ptr;
	ptr = &EstadosFinales;
	tablaAFD = afntoafd( tablaAFN, alf, ren, col, ptr, EstadoFinal);	//obteniendo la tabla generamos el analisis 
	//analizador Lexico
	Lexico(tablaAFD, EstadosFinales, alf, ren, col);
	//algoritmo de Primeros y Siguientes
	primeros_siguientes(); 
	//algoritmo de la colecci·n canonica. 
	coleccion_canonica();
	//al final del algoritmo de la colecci·n can·nica se genera la
	//tabla de analisis_sintactico y posteriormente el Analisis Sintactico LR
	return 0;
}


