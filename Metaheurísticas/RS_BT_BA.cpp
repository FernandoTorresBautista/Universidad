/* VRP */
#include<stdio.h>
#include<iostream>
#include<algorithm>
#include<fstream>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<vector>
#include<map>
#include<time.h>
#include<sys/time.h>
using namespace std;

class VRP{
	public:
		int _depot_;		//N�mero del nodo dep�sito/principal
		int K;  			//n�mero de veh�culos
		int Q;  			//Capacidad general de los veh�culos
		int N;  			//N�mero de nodos clientes
		float costoRutaG;
		float **Nodos;
		float **Distancias;
		float *capacidadesRutas;
		vector< vector<float> > Rutas;
		void initRutas( int k );
		void initCapacidades( int k );
		void generaMatriz(int gen, int filas, int col);
		void liberar();
};
VRP problem_VRP;

void VRP::initRutas( int k){
	vector<float> aux;
	for(int i=0; i<k; i++){
		this->Rutas.push_back( aux );
	}
}

void VRP::initCapacidades( int k ){
	this->capacidadesRutas = new float[ k ];
	for (int i = 0; i < k; i++) {
		this->capacidadesRutas[i] = 0.0;
	}
}
void VRP::generaMatriz(int gen, int filas, int col){
	if( gen==0 ){
		this->Nodos = new float*[ filas ];
		for (int i = 0; i <= filas; i++) {
			this->Nodos[i] = new float[ col ];
		}
		for (int i = 0; i <= filas; i++) {
		    for (int j = 0; j < col; j++) {
		        this->Nodos[i][j] = 0;
		    }
		}
	}
	if(gen==1){
		this->Distancias = new float*[ filas ];
		for (int i = 0; i <= filas; i++) {
			this->Distancias[i] = new float[ col ];
		}
		for (int i = 0; i <= filas; i++) {
		    for (int j = 1; j <= col; j++) {
		        this->Distancias[i][j] = 0;
		    }
		}
	}
}

/* Libera la memoria que ocupa la clase VRP*/
void VRP::liberar(){
	if(Nodos!=NULL){
		for(int n = 0; n<=this->N ; n++) this->Nodos[n]=NULL;		//borrar memoria de Nodos
		for(int n = 0; n<=this->N ; n++) this->Distancias[n]=NULL;	//borrar memoria de Distancias
	}
	if( this->capacidadesRutas!=NULL) {
		this->capacidadesRutas=NULL;								//borrar memoria de Rutas
	}
	if( !this->Rutas.empty() ){
		this->Rutas.clear();
	}
}

/* Lectura del archivo de entrada */
void leerArchivo(string archivo){
	int cont = 0, num=1;
	string cadena, sub;
	ifstream fe( archivo.c_str() );
	while( !fe.eof() ) {
		getline(fe, cadena);
		if( cont==0 ){		
            cout << "Archivo: " << cadena << endl;
			sub = cadena.substr( cadena.find("k")+1 , cadena.length() );
			problem_VRP.K = atoi( sub.c_str() );
		}
		if( cont==3 ){		
			sub = cadena.substr( cadena.find(":")+1 , cadena.length()-1 );
			problem_VRP.N = atoi( sub.c_str() );
		}
		if( cont==5 ){		
			sub = cadena.substr( cadena.find(":")+1 , cadena.length()-1 );
			problem_VRP.Q = atoi( sub.c_str() );
			problem_VRP.generaMatriz( 0, problem_VRP.N , 4 );
		}
		if( cont>6 && cont< problem_VRP.N+7 ){  
			if(cadena.at(0)==' ' ){
				cadena = cadena.substr(1, cadena.length());
				sub = cadena.substr( 0 , cadena.find_first_of(" ")+1 );
				problem_VRP.Nodos[num][0] = atoi( sub.c_str() );
				sub = cadena.substr( cadena.find_first_of(" ")+1 , cadena.find_last_of(" ")+1 );
				problem_VRP.Nodos[num][1] = atoi( sub.c_str() );
				sub = cadena.substr( cadena.find_last_of(" ")+1 , cadena.length() );
				problem_VRP.Nodos[num][2] = atoi( sub.c_str() );
			}
			else{
				sub = cadena.substr( 0 , cadena.find_first_of(" ") );
				problem_VRP.Nodos[num][0] = atoi( sub.c_str() );
				sub = cadena.substr( cadena.find_first_of(" ") , cadena.find_last_of(" ") );
				problem_VRP.Nodos[num][1] = atoi( sub.c_str() );
				sub = cadena.substr( cadena.find_last_of(" ") , cadena.length() );
				problem_VRP.Nodos[num][2] = atoi( sub.c_str() );
			}
			num++;
		}
		if(cont == problem_VRP.N+7){ num=1; }
		if(cont > problem_VRP.N+7 && cont < ( 2* problem_VRP.N + 8 ) ){
			sub = cadena.substr( cadena.find(" ") , cadena.length() );
			problem_VRP.Nodos[num][3] = atoi( sub.c_str() );
			num++;
		}
		if( cont == ( 2* problem_VRP.N + 9 ) ){
			problem_VRP._depot_ = atoi( cadena.c_str() );
		}
		cont++;
	}
	fe.close();
}

/* Calcular distancias eulerianas */
void calcularDistancias(){
	problem_VRP.generaMatriz( 1, problem_VRP.N, problem_VRP.N );
	for(int i=1; i<=problem_VRP.N; i++){
		for(int j=1; j<=problem_VRP.N; j++){
			if( i!=j && i!=0 && j!=0 ){
				problem_VRP.Distancias[i][j] = sqrt( pow((problem_VRP.Nodos[i][1] - problem_VRP.Nodos[j][1]), 2) + pow(( problem_VRP.Nodos[i][2] - problem_VRP.Nodos[j][2]), 2) );
			}
		}
	}
}

/* Retorna una permutaci�n aleaotoria de un arreglo de int(nodos con posicion x e y) para la soluci�n */
void permutacionAleatoria(int v[], int n){
	int i,k,temp;
	for(i=1;i<=n;i++){
		k = 1+rand()%(n-1);
		temp=v[i];
		v[i]=v[k];
		v[k]=temp;
	}
}

/* Obtiene una soluci�n inicial*/
vector<vector< float > > SolucionInicial(  ){
	int cad[ problem_VRP.N ];
	for( int a=1; a<=problem_VRP.N; a++ ){ cad[a]= a; }
	permutacionAleatoria(cad, problem_VRP.N);
	vector<vector< float > > VV;
	vector< float > Aux;
	vector< float > C;
	for (int i = 0; i < problem_VRP.K; i++) {
		C.push_back( 0.0 );
	}
	int k=0;
	for(int i=1; i<=problem_VRP.N ; i++ ){
		if( cad[i] != problem_VRP._depot_ ){
			if( (problem_VRP.Nodos[ cad[i] ][3] + C[k] ) <= problem_VRP.Q ){
				C[k] += problem_VRP.Nodos[ cad[i] ][3];
				Aux.push_back( cad[i] );
			}
			else{
				k++;
				VV.push_back( Aux );
				Aux.clear();
				if( k<problem_VRP.K ){
					if( (problem_VRP.Nodos[ cad[i] ][3] + C[k] ) <= problem_VRP.Q ){
						C[k] += problem_VRP.Nodos[ cad[i] ][3];
						Aux.push_back( cad[i] );
					}
				}
			}
		}
	}
	return VV;
}

/* Retorna el costo de rita de un vector de vectores */
float costoVR( vector<vector <float> >R){
	float costo  = 0.0;
	vector< vector<float> >::iterator row;
	vector< float >::iterator col;
	int actual = problem_VRP._depot_;
	for (row = R.begin(); row != R.end(); row++ ) {
		costo += problem_VRP.Distancias[ actual ][  int(*( row->begin() )) ] ;
		for ( col = row->begin()+1; col != row->end(); col++) {
			costo += problem_VRP.Distancias[ actual ][ int(*col)  ] ;
			actual = *col;
		}
		costo+= problem_VRP.Distancias[ actual ][ problem_VRP._depot_];
		actual = problem_VRP._depot_;
	}
	return costo;
}

/* Imprime un vector de vectores */
void imprimirVR( vector<vector <float> >R){
	vector< vector<float> >::iterator row;
	vector< float >::iterator col;
	int k=0;
	for (row = R.begin(); row != R.end(); row++) {
		cout<<"\n" << k << " --> ";
		for (col = row->begin(); col != row->end(); col++) {
			cout << " "<< *col ;
		}
		k++;
		if( k == problem_VRP.K ){return;}
	}
}

/* Copia el vector de Rutas en un nuevo vector de vectores */
void copiarVR( vector< vector<float> > *nuevo ){
	vector <float> N;
	vector< vector<float> >::iterator row;
	vector< float >::iterator col;
	int k=0;
	for (row = problem_VRP.Rutas.begin(); row != problem_VRP.Rutas.end(); row++, k++) {
	   	for (col = row->begin(); col != row->end(); col++) {
			N.push_back( *col );
		}
		nuevo->push_back( N );
		N.clear();
	}
}

/* Retorna un valor aleatorio entre 0 y n*/
int ranCU( int n ){
	return (int)(rand()%n);
}

/* Verifica la capacidad de una ruta en particular */
bool verificaRuta( vector<float>ruta ){
	//bool fun = false;
	vector<float>::iterator it;
	float suma=0.0;
	for(int i=0; i<int( ruta.size() ); i++){
		suma += problem_VRP.Nodos[ int(ruta.at(i)) ][3];
		if(suma > problem_VRP.Q){
			return false;
		}
	}
	return true;
}

float costoR(vector<float>R){
	vector<float>::iterator it;
	float suma=0.0;
	int i, 
		actual=problem_VRP._depot_;
	suma+=problem_VRP.Distancias[actual][int(R[0])];
	actual = int(R[0]);
	for(i=1; i< int( R.size() ) ; i++){
		suma += problem_VRP.Distancias[ actual ][ int(R[i]) ];
		actual = R[i];
	}
	suma+=problem_VRP.Distancias[actual][problem_VRP._depot_];
	return suma;
}

/* Busca un elemento en un vector */
bool inVector(vector<float>V, float A){
	vector< float >::iterator it;
	it = find ( V.begin(), V.end(), A);
	if( it==V.end() ){
		return false;
	}
	return true;
}

/* retorna un numero aleatoria 0 o 1 */
float r_(){
	return float(rand() % 101)/100;
}

int ruo(){
	if(r_()<0.5) {return 0;}
	return 1;
}

/* Copia un vector de vectores en otro */
void copiaVVtoVV(vector< vector<float> > &A, vector< vector<float> > &B){
	B.clear();
	for(int i=0; i<int( A.size() ); i++){
		B.push_back(A[i]);
	}
}

/* Copiar un vector del vector de Rutas que se asigne como entrada */
void copiaRfromVR( vector< vector< float > > &A,vector<float> &nuevo, int nRuta ){
	for( int i=0; i< int( A[nRuta].size() ) ; i++){
		nuevo.push_back( A[nRuta][i] );
	}
}

/* Función que modifica un vecino con M% de modificación de S */
vector< vector<float> > calcularVecino( float M, vector< vector<float> > &Mod){
	vector< vector<float> > modificable;
	for(int i=0; i<int( Mod.size() ); i++){
		modificable.push_back( Mod[i] );
	}
	int num = int( M*problem_VRP.N );
	if( (num%2) != 0 ){ num++; }		//Para que los cambios que realize siempre sean par 
	vector< float > cambios;		
	while( int( cambios.size() ) < num ){		//Mientras no se modifique el % M de S
		if( ruo() ){					//Cambio en una ruta
			int ruta_, n1, n2;
			do{
				ruta_ = ranCU( problem_VRP.K-1 );
				n1 = ranCU( modificable[ruta_ ].size()-1 );
				n2 = ranCU( modificable[ruta_ ].size()-1 );
				if( inVector( cambios, modificable[ruta_ ][ n1 ] )==true || 
					inVector( cambios, modificable[ruta_ ][ n2 ] )==true ){
					n1 = n2;
				}
			}while(n1 == n2);	
			vector<float>nuevo;
			copiaRfromVR( modificable, nuevo, ruta_ );	
			swap( nuevo[n1], nuevo[n2] ); 
			//if( costoR(nuevo)<= costoR(modificable[ruta_]) ){
				cambios.push_back( modificable[ruta_][n1] );
				cambios.push_back( modificable[ruta_][n2] );
				swap( modificable[ruta_][n1], modificable[ruta_][n2] );
			//}
		}
		else{						//cambio entre rutas
			bool vects = false;
			do{
				int ruta_1, ruta_2, n1, n2;
				do{
					ruta_1 = ranCU( problem_VRP.K-1 );
					ruta_2 = ranCU( problem_VRP.K-1 );
					n1 = ranCU( modificable[ruta_1 ].size()-1 );
					n2 = ranCU( modificable[ruta_2 ].size()-1 );
					if( inVector( cambios, modificable[ruta_1 ][ n2 ] )==true || 
						inVector( cambios, modificable[ruta_2 ][ n2 ] )==true ){
						n1 = n2;
					}
				}while(n1 == n2);	
				vector < float >nuevo1, nuevo2;		
				copiaRfromVR( modificable, nuevo1, ruta_1 );	
				copiaRfromVR( modificable, nuevo2, ruta_2 );	
				swap( nuevo1[n1], nuevo2[n2] ); 	//Cambio de los nodos
				if( verificaRuta( nuevo1 )==true && verificaRuta( nuevo2 )==true ){		//Verificar la capacidad de las rutas
					//VErificar si el costo de ruta es mejor
					vects = true;
					//if( costoR(nuevo1)<=costoR(modificable[ruta_1]) && costoR(nuevo2)<=costoR(modificable[ruta_2]) ){
						cambios.push_back( modificable[ruta_1][n1] );
						cambios.push_back( modificable[ruta_2][n2] );
						swap( modificable[ruta_1][n1], modificable[ruta_2][n2] );
					//}
				}
				nuevo1.clear();
				nuevo2.clear();
			}while( vects==false );
		}
	}
	cambios.clear();
	return modificable;
}

float factorial(int n){
    float resp;
    if ((n==0)||(n==1)) return(1);
    resp=factorial(n-1)*n;
    return(resp);
}

float euler(){
	float   e=0,
			error=0.0000001,
			epsilon=error+1;
    int a=0;   
    while (error<epsilon){
        e=e+(1/(factorial(a)));
        a++;
        epsilon=(1/(factorial(a)));
    }
    return e;
}

/* retorna "a - b" en segundos */
double timeval_diff(struct timeval *a, struct timeval *b){
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}

/*  Recocido simulado */
void recocidoSimulado(float tI, float tF, float RA, int n, float decT){
	struct timeval t_ini, t_fin;
	double secs;
	vector<double>Tiempos;
	// Temperatura inicial, T final, Variable de T, razón de aceptación
	float tInicial = tI, tFinal = tF, Temperatura = tInicial,razonAceptacion = RA, cBVecino = 0.0, e = 0.0;
	problem_VRP.Rutas = SolucionInicial();
    cout <<"Ruta inicial \n";
    imprimirVR( problem_VRP.Rutas );
    float cInicial = problem_VRP.costoRutaG = costoVR( problem_VRP.Rutas );
    cout << "Costo inicial = " << cInicial << endl;	
	e = euler( );
	while( Temperatura>tFinal /* && iteracion<MaxIteraciones*/ ){	
		gettimeofday(&t_ini, NULL);
		vector< vector<float> > auxR1, mejor;
		bool first = false;
		int it=0;	
		while( it < n ){					//Calcular N(M) vecinos con M % de modificación de S
			auxR1 = calcularVecino( razonAceptacion, problem_VRP.Rutas );
			if( first ){
				if( cBVecino >= costoVR( auxR1 ) ){
					if(!mejor.empty()){ mejor.clear(); }
					copiaVVtoVV( auxR1, mejor );
					cBVecino=costoVR( mejor );
				}
			}else{
				if(!mejor.empty()){ mejor.clear(); }
				copiaVVtoVV( auxR1, mejor );
				cBVecino=costoVR( mejor );
				first=true;
			}
			it++;
			auxR1.clear();
		}
		float dE = costoVR(mejor) - problem_VRP.costoRutaG;
		if( dE <= 0 ){
			problem_VRP.Rutas.clear();
			problem_VRP.costoRutaG = costoVR(mejor);
			copiaVVtoVV(mejor, problem_VRP.Rutas);
			mejor.clear();
		}
		else{
			float alpha = r_();
			float e_div = pow( e, dE / Temperatura) ;
			if( alpha <= e_div ){
				problem_VRP.Rutas.clear();
				problem_VRP.costoRutaG = costoVR(mejor);
				copiaVVtoVV(mejor, problem_VRP.Rutas);
				mejor.clear();
			}
		}
		Temperatura -= decT;
		gettimeofday(&t_fin, NULL);
		secs = (1000)*(timeval_diff(&t_fin, &t_ini));
		Tiempos.push_back(secs);
	}
	cout << " Ruta final : " << endl;
	imprimirVR( problem_VRP.Rutas );
	cBVecino = problem_VRP.costoRutaG = costoVR( problem_VRP.Rutas );
	cout << " Costo final = " << cBVecino << endl;
	double suma=0;
	cout << " tiempos por iteración(i) " <<endl;
	for(int i=0; i< int(Tiempos.size()); i++){
		suma += Tiempos.at(i);
		cout << " t("<< (i+1) << ") = " << Tiempos.at(i) << " ms" <<endl;
	}
	cout << "Promedio de ejecución = " << ( suma/Tiempos.size() ) << " ms" <<endl;
}

/****************************************************************************************************************/

// crea a un vecindario de nuevos vecinos 
map<int, vector< vector<float> > > init_SNeighborhood( int N, float M, vector<vector<float> >&S ){
	map< int, vector< vector<float> > > sN;
	vector< vector<float> > auxR1;
	for(int i=0; i<N; i++){
		copiaVVtoVV( S, auxR1 );
		calcularVecino( M, auxR1 );
		sN.insert ( pair<int, vector<vector<float> > >( i , auxR1 ) );
		auxR1.clear();
	}
	return sN;
}

bool equals( vector<vector<float> > &s, vector<vector<float> > &s2 ){
	int j, k;
	for( j=0; j<int(s.size()); j++){
		for( k=0; k< int(s[j].size()); k++ ){
			if( s[ j ][k] != s2[j][k] ){
				return false;
			}
		}
	}
	return true;
}

bool notContainTE( vector<vector<float> > &s, map< int, vector<vector<float> > > &TL){
	if( TL.empty() ){	//Si esta vacia no hay nada
		return true;
	}
	else{				//Si no esta vacia comparar cada vector de vectores
		for( int i=0; i< int( TL.size() ); i++){
			vector< vector<float> > comp = TL.at(i);
			if( equals(s, comp) ){
				return false;
			}
		}
	}
	return true;
}

// agrega un vector de vectores a un mapa 
void candidateListAdd( map< int, vector<vector<float> > > *clA, vector<vector<float> > &s ){	
	int pos = clA->size();
	clA->insert ( pair<int, vector<vector<float> > >( pos , s ) );
}

// Retorna la posición del mejor candiadto 
int localBestCandidate( map< int, vector<vector<float> > > &cl ){
	if(cl.empty()){
		return -1;
	}
	float mejor=costoVR( cl.at(0) );
	int pos;
	for( int i=1; i<int( cl.size() ); i++){
		float cost = costoVR( cl.at(i) );
		if( mejor > cost ){
			swap(mejor, cost);
			pos = i;
		}
	}
	return pos;
}

void featureDifference( vector<vector<float> >  &sC, map< int, vector<vector<float> > > *TL){
	int pos = TL->size();
	TL->insert ( pair<int, vector<vector<float> > >( pos , sC ) );
}

void expireFeature(map< int, vector<vector<float> > > *TL){
	TL->clear();
}

void Tabu( float RA, int maxIT, int n, int tlN ){
	struct timeval t_ini, t_fin;
	double secs;
	vector<double>Tiempos;
    int it=0,
		max_it= maxIT,
		N= n,
		maxTabuListSize = int( problem_VRP.N*tlN );//tlN es para definir el maximo de la lista 2N, 3N
	float M=RA;
	problem_VRP.Rutas = SolucionInicial();
	vector< vector<float> > S;												//sBest, S;
	copiaVVtoVV( problem_VRP.Rutas, S);										// 2. SBest <-- S 
	cout <<"\nRuta inicial \n";
	imprimirVR( problem_VRP.Rutas );
	float cInicial = costoVR( problem_VRP.Rutas );
	cout << "Costo inicial = " << cInicial<< endl;	
	map< int, vector<vector<float> > >TabuList, CandidateList;				// 3. TabuList <--Null, 4.1
	cout << "inicializando" << endl;	
	while( it<=max_it ){													// 4. 
		gettimeofday(&t_ini, NULL);
		map< int, vector<vector<float> > > sNeighborhood;					// *4.1
		sNeighborhood = init_SNeighborhood(N, M, S);					// 4.2	Inicializar vecindario
		vector< vector<float> > sCandidate;									// 4.3
		for(int i=0; i<N; i++){
			sCandidate = sNeighborhood.at(i);
			if( notContainTE(sCandidate, TabuList) ){						// 4.3.1
				candidateListAdd( &CandidateList, sCandidate );				// 4.3.1.1
			}
			if(!sCandidate.empty())
				sCandidate.clear();
		}
		int pMap = localBestCandidate( CandidateList );
		sCandidate = CandidateList.at(pMap);				// 4.4 sCandidate <-- localBestCandidate(CL);
		copiaVVtoVV( sCandidate, S);					// 4.5 S <-- sCandiadte
		if( costoVR( sCandidate ) <= costoVR( problem_VRP.Rutas ) ){// 4.6 if(fitness() > fitness(sBest))
			featureDifference( sCandidate, &TabuList );	// 4.6.1 TL <-feactDiff( sCandidate, sBest );
			copiaVVtoVV( sCandidate, problem_VRP.Rutas);					// 4.6.2
			if( int( TabuList.size() ) > maxTabuListSize ){						// 4.6.3
				expireFeature( &TabuList );							// 4.6.3.1 ExpireFeacture(TabuList)
			}
		}
		it++;														// 4.7 Aumentar iteraciones
		if( !sNeighborhood.empty() ){ sNeighborhood.clear(); }
		gettimeofday(&t_fin, NULL);
		secs = (1000)*(timeval_diff(&t_fin, &t_ini));
		Tiempos.push_back(secs);
	}
	cout <<"\nRuta final \n";
	imprimirVR( problem_VRP.Rutas );
	cout << "Costo final = " << costoVR( problem_VRP.Rutas ) << endl;
	double suma=0;
	cout << " tiempos por iteración(i) " <<endl;
	for(int i=0; i<int( Tiempos.size() ); i++){
		suma += Tiempos.at(i);
		cout << " t("<< (i+1) << ") = " << Tiempos.at(i) << " ms" <<endl;
	}
	cout << "Promedio de ejecución = " << ( suma/Tiempos.size() ) << " ms" <<endl;
} 

map< int, vector<vector<float> > > init_HM( int N){
	map< int, vector<vector<float> > > HM;
	for(int i=0; i<N; i++){
		cout << "Calcular vecino N" << endl;
		vector< vector<float> >Nuevo = SolucionInicial();
		cout << "solucion inicial " << endl;
		HM.insert( pair<int, vector<vector<float> > >( i , Nuevo) );
		//HM.insert( pair<int, vector<vector<float> > >( i , SolucionInicial( ) ) );
	}
	return HM;
}

int getWorse(vector<vector<float> > &V, map< int, vector<vector<float> > > &HM, int N){
	bool f=true;
	float pC;
	int j = 0;
	for(int i=0; i<N; i++){
		vector< vector< float> > A = HM.at(i);
		float cA = costoVR( A );
		if(f){
			pC = cA;
			j=i;
			f=false;
		}
		else{
			if( pC > cA ){
				V.clear();
				copiaVVtoVV( HM.at(i), V );
				pC=cA;
				j=i;
			}
		}
	}
	return j;
}

void Armonica(int max_it, float M){
	struct timeval t_ini, t_fin;
	double secs;
	vector<double>Tiempos;
	int it=0;
	map< int, vector<vector<float> > > HM;				//
	vector< vector<float> >Nuevo, Peor;
	int N = problem_VRP.N*2;
	for(int i=0; i<N; i++){
		HM.insert( pair< int, vector<vector<float> > >(i, SolucionInicial() ) );
	}
	while( it<max_it ){									// 2. while( it< itMax) 
		gettimeofday(&t_ini, NULL);			
		if( ruo() == 0 ){								// 2.1 Elegir una nueva armonia random de o y 1
			Nuevo = SolucionInicial();
		}
		else{
			int sol_ = ranCU( N-1 );
			copiaVVtoVV( HM.at(sol_), Nuevo );// 2.1.2 random=1, modifica una solucion de la memoria
			calcularVecino( M, Nuevo );
		}
		int pos = getWorse(Peor, HM, N);				// 2.2 Si es mejor la Nueva>Peor 
		if( costoVR(Peor) > costoVR(Nuevo) ){
			copiaVVtoVV( Nuevo, HM.at(pos) );			//  Se sustituye 
		}
		it++;
		gettimeofday(&t_fin, NULL);
		secs = (1000)*(timeval_diff(&t_fin, &t_ini));
		Tiempos.push_back( secs );
		Nuevo.clear();
		Peor.clear();
	}
	int pMejor = localBestCandidate( HM );
	imprimirVR( HM.at(pMejor) );
	cout << "Costo final de la mejor ruta generada :" << costoVR( HM.at(pMejor) ) << endl;
	double suma=0;
	cout << " tiempos por iteración(i) " <<endl;
	for(int i=0; i<int( Tiempos.size() ); i++){
		suma += Tiempos.at(i);
		cout << " t("<< (i+1) << ") = " << Tiempos.at(i) << " ms" <<endl;
	}
	cout << "Promedio de ejecución = " << ( suma/Tiempos.size() ) << " ms" <<endl;
}
/******************************************************************************************/






































//void seleccion(map< int, vector<vector<float> > > &Pi, map< int, vector<vector<float> > > *Pf, float PS, int N){
map< int, vector<vector<float> > >seleccion(map< int, vector<vector<float> > >&Pi, float PS, int N){
	int selec = int( N * PS);
	if( (selec%2) != 0 ){ selec++; }
	vector< float >nSPI;
	int pTorneo = 0.3 *N ;					// Checar si el porcentaje de N para cada Torneo y selecciond
	if( (pTorneo%2) != 0 ){ pTorneo++; }
	int j=0;
	map< int, vector<vector<float> > > Pf;
	while( Pf.size()< selec ){
		int i=0;					// Seleccionar N elementos de Pi, y obtener el mejor por un torneo
		vector<vector<float> > Best;	// Mejor de N elegidos al azar
		do{								// Torneo en un while eligiendo el mejor de n al azar 
			int P1, P2, P;
			do{
				P1 = ranCU( Pi.size()-1 );
				P2 = ranCU( Pi.size()-1 );
			}while(P1==P2);
			if( (costoVR( Pi.at(P1)) <= (costoVR( Pi.at(P2) ) ) ) ){
				copiaVVtoVV( Pi.at(P1), Best );
			}
			else{
				copiaVVtoVV( Pi.at(P2), Best );
			}
			i++;
		}while( i < pTorneo );
		Pf.insert( pair<int, vector<vector<float> > >( j, Best ) );//en Pf el mejor de cada torneo 
		j++;
	}
	return Pf;
}

/* Inicializa la mascara de un Vector de Vectores */
void init_mask( vector< vector<bool> > &M, vector< vector< float > >&P){
	vector<bool>auxM;
	for(int i=0; i<problem_VRP.K; i++){
		M.push_back( auxM );
	}
	for(int i=0; i<P.size(); i++){
		for(int j=0; j<P[i].size(); j++){
			M[i].push_back( false );
		}
	}
}

/* Crea la mascara del vector y te devuelve en v<v<float>Nodos cada nodo a cambiar  */
void creaMask( vector<vector<float> > &P, vector<vector<bool> > &mask, int &cruces, vector< float >&Nodos){
	int c=0;
	while( c < cruces ){
		for(int i=0; i<P.size(); i++){
			for(int j=0; j<P[i].size(); j++){
				if( r_()<.3 ){
					if( c < cruces ){
						mask[i][j] = true;
						Nodos.push_back( P[i][j] );
						c++;
						if(c==cruces){
							return;
						}
					}
				}
			}
		}
	}
}

/* Reemplaza en Hijo lo que el Padre tiene del Vector Nodos en el orden de aparicion de cada nodo */
void reemplaza(vector<vector<float> > &H, vector<vector<float> > &P, vector<vector<bool> >&mask, vector<float> &Nodos ){
	vector<float>Orden;
	for(int i=0; i<P.size(); i++){
		int a=P[i].size();
		for(int j=0; j<a; j++){
			if( inVector(Nodos, P[i][j])==true ){
				Orden.push_back( P[i][j] );
			}
		}
	}
	vector<float>::iterator it = Orden.begin();
	for(int i=0; i<mask.size(); i++){
		for(int j=0; j<mask[i].size(); j++){
			if( mask[i][j]==true ){
				float aux = *it;
				swap(H[i][j], aux);
				it++;
				if(it==Orden.end()){
					i=mask.size();
				}
			}
		}
	}
}

bool verificaH( vector< vector<float> > &H ){
	for(int i=0; i<H.size(); i++){
		if( costoR( H[i] )>problem_VRP.Q ){
			return false;
		}
	}
	return true;
}

/* Copia un map de vectores en otros */
void copiaMap( map< int, vector<vector<float> > > &A, map< int, vector<vector<float> > > &B){
	B.clear();
	for(int i=0; i<A.size(); i++){
		B.insert( pair<int, vector<vector<float> > >(i, A[i]) );
	}
}

/* CrossOver VRP */
map< int, vector<vector<float> > > cruce(map< int, vector<vector<float> > > &PR, float PC, int N){
	map< int, vector<vector<float> > >P;
	copiaMap(PR,P);
	cout << "Se copia " << PR.size()<<endl;
	vector< vector<float> > P1, P2, H1, H2;
	vector< vector<bool> >mask1, mask2;
	int cruces = int(PC * N);
	if( cruces%2 != 0){ cruces++; }
	int last = P.size();
	while( P.size() < N ){
		if(last!=P.size()){
			cout << "Poblacion size(): "<< P.size() << endl;
			last = P.size();
		}
		int pos1 = ranCU( P.size()-1 );				// Selecciono dos padres al azar 
		int pos2 = ranCU( P.size()-1 );
		P1 = P.at( pos1 );
		P2 = P.at( pos2 );
		H1 = P.at( pos1 );
		H2 = P.at( pos2 );
		init_mask( mask1, P1 );
		init_mask( mask2, P2 );
		vector< float >N1, N2;
		creaMask(P1,mask1, cruces, N1);
		creaMask(P2,mask2, cruces, N2);
		//Una vez creadas las mascaras, cambiar los nodos en el orden de aparicion en el padre contrario
		reemplaza( H1, P2, mask1, N1);
		reemplaza( H2, P1, mask2, N2);
		//se verifica que la ruta cumpla con la capacidad por cada ruta para ser parte de la poblacion
		//if( verificaH(H1) ){
			//cout << "H1" << endl;
			if( rand() % 100 < PC*100 ){
				int pos = P.size();
				P.insert( pair<int, vector< vector<float> > >(pos, H1) );
			//}
		}
		//if( verificaH(H2) ){
			//cout << "H2" << endl;
			if( rand() % 100 < PC*100 ){
				int pos = P.size();
				//pos = P.size();
				P.insert( pair<int, vector< vector<float> > >(pos, H1) );
			//}
		}
		P1.clear();P2.clear();
		H1.clear();H2.clear();
		mask1.clear();mask2.clear();
	}
	return P;
}

/* Mutación */ // Aplico mutación por intercambios 
map< int, vector<vector<float> > > mutacion(map< int, vector<vector<float> > > &P, float PM, int cambioMut){
	map< int, vector<vector<float> > >N;
	vector<vector<float> >Nuevo;
	for(int i=0; i<P.size(); i++){
		if( rand() % 100 < PM*100 ){
			copiaVVtoVV( P[i], Nuevo );
			calcularVecino( cambioMut, Nuevo );
			N.insert( pair<int, vector< vector<float> > >(i, Nuevo) );
		}
		else{
			N.insert( pair<int, vector< vector<float> > >(i, Nuevo) );
		}
	}
	return N;
}

void Genetico( int N, int nMaxG, float PorcSdeN, float PorcC, float PorcM, float cambioMut ){
	// N, nGeneraciones, % seleccion de N, % de cruce, % mutación, % de cambio de mutación 
	int gen=1;
	map< int, vector<vector<float> > > PoblacionInicial;
	map< int, vector<vector<float> > > P;
	for(int i=0; i<N; i++){
		PoblacionInicial.insert( pair< int, vector<vector<float> > >(i, SolucionInicial() ) );
	}
	int pMejo = localBestCandidate( PoblacionInicial );
	imprimirVR( PoblacionInicial.at(pMejo) );
	cout << "\nCosto inicial de la mejor ruta generada :" << costoVR( PoblacionInicial.at(pMejo) ) << endl;
	//init_HM( &PoblacionInicial, N);	// Inicializa una poblacion de N individuos de forma aleatoria
	while( gen <= nMaxG ){
		cout << "\n\nGeneración :" << gen << endl;
		
		cout << "Selección "<<endl;
		cout << "antes "<<  PoblacionInicial.size() ;
		P = seleccion( PoblacionInicial, PorcSdeN, N  );// Seleccion por torneo de PorcSdeN vecinos de PI
		cout << "//despues"<<  P.size() <<endl;
		for(int i=0; i<P.size(); i++){
			cout << "*****************************" << endl;
			imprimirVR( P.at(i) );
			cout<<endl;
		}
		
		PoblacionInicial.clear();
		cout << "Cruce "<<endl;
		cout << "antes "<<  PoblacionInicial.size() ;
		PoblacionInicial = cruce( P, PorcC, N);
		cout << "//después "<<  PoblacionInicial.size()<<endl ;
		
		
		P.clear();
		cout << "Mutación "<<endl;
		//mutacion( PoblacionInicial, PorcM, cambioMut );
		cout << "antes: "<<P.size() << endl;
		P = mutacion( PoblacionInicial, PorcM, cambioMut );
		cout << "despues: "<<P.size() << endl;
		gen++;
		copiaMap( P, PoblacionInicial );
		P.clear();
		
		cout << "P.size(): "<<P.size()<<endl;
		cout << "PoblacionInicial.size(): "<< PoblacionInicial.size()<<endl;
		
	}
    int pMejor = localBestCandidate( PoblacionInicial );
	imprimirVR( PoblacionInicial.at(pMejor) );
	cout << "\nCosto final de la mejor ruta generada :" << costoVR( PoblacionInicial.at(pMejor) ) << endl;
	PoblacionInicial.clear();
}


/* Main */
int main(void){
	int opcion=-1;
	srand( time(0) ); 
	while( opcion!=100 ){
		cout << "_________________________________________________________________" << endl;
		cout << "¿Que algoritmo desea ejecutar ?"  <<endl;
		cout << " Opciones:\n 1. Recocido simulado\n 2. Busqueda Tabu \n 3. Busqueda armonica \n 4. Genetico \n ";
		cin >> opcion;
		cout << "_________________________________________________________________\n\n" << endl;
		cout << "Introduca el nombre del archivo de prueba (*.ext): " <<endl;
		string archivo = "";
		cin >> archivo;
		leerArchivo( archivo );
		calcularDistancias();
		switch(opcion){
			case 1:{	//void recocidoSimulado(float tI, float tF, float RA, int n, float decT);
				float tI, tF, RA, decT;
				int n;
/*				cout << "\n\nIngrese los parametros: " <<endl;
				cout << "Temperatura inicial: "; cin >> tI;
				cout << "Temperatura final: "; cin >> tF;
				cout << "Razon de aceptacion: "; cin >> RA;
				cout << "N vecinos: "; cin >> n;
				cout << "Decremento de temperatura: "; cin >> decT;
				recocidoSimulado( tI, tF, RA, n,decT);*/
				recocidoSimulado( 100, 5, 0.2, 20, 4.0);
				problem_VRP.liberar();
				opcion = -1;
				cout << endl;
				break;}
			case 2:{	//void Tabu( float RA, int maxIT, int n, int tlN )==( % modificacion S, max Iteraciones, N(para n vecinos de S), tabulistN(cuantas N) )
/*				float PorcM, max_it, tlN ;
				int n;
				cout << "\n\nIngrese los parametros: " <<endl;
				cout << "M Porcentaje de modificacion de S: "; cin >> PorcM;
				cout << "Iteraciones maximas: "; cin >> max_it;
				cout << "N, para generar N vecinosde S: "; cin >> n;
				cout << "N, para tabuList*(N): "; cin >> tlN;*/
				Tabu(0.2, 100, 20, 2);
				//Tabu(PorcM, max_it, n, tlN);
				problem_VRP.liberar();
				opcion = -1;
				break;}
			case 3:{	//void Armonica(int max_it, float M) M % modificacion en random=1
				/*int max_it;
				float M;
				cout << "\n\nIngrese los parametros: " <<endl;
				cout << "Iteraciones maximas: "; cin >> max_it;
				cout << "M Porcentaje de modificacion de S: "; cin >> M;
				Armonica(max_it, M);*/
				Armonica(100, 0.2);
				problem_VRP.liberar();
				opcion = -1;
				break;}
			case 4:{
				//void Genetico( int N, int nMaxG, float PorcSdeN, float PorcC, float PorcM, float cambioMut )
				Genetico( problem_VRP.N, 3, 0.6, 0.3, .2, 0.15);
				cout <<"Sale genético"<<endl;
			}
		}
		cout << "¿Desea realizar una nueva ejecucion?(y/n) \n" ;
		char end_;
		cin >>end_;
		if(end_=='n'){ opcion=100; }
	}
	return EXIT_SUCCESS; 
}








