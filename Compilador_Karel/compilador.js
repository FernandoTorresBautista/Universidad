/**
 * @description {
 *  Compilador del lenguaje basado en Karel el robot, en una versión en español en C.  
 * }
 * @author Fernando Torres Bautista<fernandot371@gmail.com>
 * @version 1.0
 * @const{ 
 *  Universidad Tecnológica de la Mixteca 
 * }
 */
var PALABRAS_RESERVADAS = [ "principal", "mover", "define", "si","sino", "mientras", "iterar", "apagar", "girarALaIzquierda", 
                            "recogeZumbador", "ponZumbador", "sucesor", "predecesor", "esCero", "NOT", "OR", "AND", "retornar"]
var FUNCIONES_BOOLEANAS = [ "elFrenteEsClaro", "elFrenteEstaBloqueado", "laIzquierdaEsClara", "laIzquierdaEstaBloqueda", "laDerechaEsClara", 
                            "laDerechaEstaBloqueda", "juntoAZumbador", "noJuntoAZumbador", "algunZumbadorEnLaMochila", "ningunZumbadorEnLaMochila", 
                            "viendoAlNorte", "viendoAlSur", "viendoAlEste", "viendoAlOeste", "noViendoAlNorte", "noViendoAlSur ", "noViendoAlEste", "noViendoAlOeste"]
var terminales = [ "principal", "(", ")", "Identificador", "define", "{", "}", "si", "sino", "mientras", "OR", "AND", "NOT", "esCero", "Funcion_Booleana", 
                    "predecesor", "sucesor", "iterar", "apagar", "girarALaIzquierda", "mover", "recogeZumbador", "ponZumbador", "retornar", ";", "E"]
var no_terminales = [ "S'", "Declaracion_de_programa", "[Declaracion_de_metodo]", "Declaracion_de_metodo", "Firma_de_metodo", "Parametro_opcional", "[Identificador]", 
                    "Tipo", "Argumento_opcional", "[Expresion_entera]", "Bloque", "[Expresion]", "Expresion", "[Instruccion]","Instruccion", "Expresion_de_llamada", 
                    "Expresion_Si", "[sino]", "Expresion_Mientras", "[Termino]", "Termino", "CY", "CO", "COO", "CN", "Clausula_atomica", "Expresion_entera", 
                    "Expresion_Iterar", "Expresion_Apagar", "Expresion_girarALaIzquierda", "Expresion_mover", "Expresion_recogeZumbador", "Expresion_ponZumbador", "Expresion_retorno", "Argumento_vacio", "Expresion_vacia"]
var TablaAnalisisSintactico_
var ColeccionCanonica_

var funVisit = []
var motorTokens=[] 

/**
 * Función para compilar el código otorgado por el usuario, obtiene el código directamente el HTML
*/
function compilarCodigo() {
    let [tokens, e] = analizadorLexico(document.getElementById("codigo").value.split("\n"))
    if(e==""){
        var resultado = ASLR(TablaAnalisisSintactico_, tokens, getArrayReglasAumentadas(), ColeccionCanonica_) 
        console.log( resultado ) 
    }
    else{ 
        console.log(e)
    }
}
/**
 * Función para llamar a el analizador sintáctico, retorn la tabla de análisis sintáctico y la colección canónica 
 * generada por las reglas de producción
 */
function compilarLenguaje(){
    [TablaAnalisisSintactico_, ColeccionCanonica_] = analizadorSintactico()
    console.log("OK")
}
/**
 * Retorna una línea procesada por sus tokens
 * @param {Array(String)} arrayLineas 
 */
function analizadorLexico(arrayLineas) {
    let flagBC = false
    let mensaje = ""
    let resultforline = new Array();
    for (let i = 0; i < arrayLineas.length; i++) {
        let xx = arrayLineas[i]
        if( flagBC==false && xx.indexOf("/*") !=-1 ){
            flagBC = true
            if(xx.indexOf("*/") !=-1){ 
                var c1 = xx.slice(0, xx.indexOf("/*"))
                var c2 = xx.slice(xx.indexOf("*/")+2, xx.length)
                xx = c1 +""+ c2
                flagBC = false
            }
        }
        if( flagBC==true ){ 
            if(xx.indexOf("*/") !=-1){
                xx = xx.slice(xx.indexOf("*/")+2, xx.length)
                flagBC = false
            }
        }
        if(xx.indexOf("//")!=-1){
            xx = xx.slice(0, xx.indexOf("//"))
        }
        let rr = []
        rr[i] = xx
        if( xx!=[] && flagBC==false ){
            [resultforline[i],mensaje] = tokenizar(rr[i]);
            if(mensaje!=""){
                mensaje+=" "+(i+1)
                break
            }
        }
    }
    return [resultforline,mensaje]
}
/**
 * Analizador sintáctico, retorna la TAS(Tabla de analisis sintactico) y los estados de la colección canónica
*/
function analizadorSintactico() {
    let pila = new Array()
    let reglas_produccion = getArrayReglasAumentadas()
    let _Primeros_ = Primero(reglas_produccion)
    let _Siguientes_ = Siguiente(reglas_produccion, _Primeros_)
    if (terminales.indexOf("$") == -1) { 
        terminales.push("$") 
    }
    let _EdosCC_ = ColeccionCanonica(reglas_produccion)
    let _TablaA_ = tablaAnalisis(_EdosCC_, _Siguientes_, reglas_produccion)
    return [_TablaA_, _EdosCC_]
}
/**
 * Representa un token después de ser procesado por el analizador léxico
 * @constructor 
 * @param {string} type 
 * @param {string} value 
 */
function Token(type, value) {
    this.type = type
    this.value = value
}
function isComma(ch) { return /,/.test(ch) }
function isPC(ch) { return /;/.test(ch) }
function isDigit(ch) { return /\d/.test(ch) }
function isLetter(ch) { return /[a-z]/i.test(ch) }
function isLeftParenthesis(ch) { return /\(/.test(ch) }
function isRightParenthesis(ch) { return /\)/.test(ch) }
function isLeftKey(ch) { return /\{/.test(ch) }
function isRightKey(ch) { return /\}/.test(ch) }
function isPalabraReservada(value) {
    if (PALABRAS_RESERVADAS.indexOf(value) != -1){ return true }
    return false
}
function isFuncionBooleana(value) {
    if (FUNCIONES_BOOLEANAS.indexOf(value) != -1) { return true }
    return false
}
function tokenizar(str) {
    str.replace(/\s+/g, "")
    str = str.split("");
    let result = [];
    let letterBuffer = [];
    let numberBuffer = [];
    let buffAux = "";
    let mens = ""
    str.forEach(function (char, idx) {
        if(char!=="\t")
            if (isDigit(char)) {
                if (letterBuffer.length) {
                    buffAux += char;
                }
                else {
                    numberBuffer.push(char);
                }
            } else if (isLetter(char)) {
                if (letterBuffer.length) {
                    var aux = letterBuffer.join("");
                    aux += char;
                    if (isPalabraReservada(aux)) {
                        /*result.push(new Token("Palabra reservada", aux));letterBuffer = [];buffAux = "";*/
                        if(aux !== "si"){
                            result.push(new Token("Palabra reservada", aux));
                            letterBuffer = []
                            buffAux = ""
                        }
                        else{
                            if(str[idx+1]=="n" && str[idx+2]=="o"){
                                result.push(new Token("Palabra reservada", "sino"));
                                letterBuffer = []
                                buffAux = ""
                                idx+=2
                            }
                            else{
                                result.push(new Token("Palabra reservada", aux));
                                letterBuffer = []
                                buffAux = ""
                            }
                        }
                    } else if (isFuncionBooleana(aux)) {
                        result.push(new Token("Funcion Booleana", aux));
                        letterBuffer = []
                        buffAux = ""
                    } else {
                        buffAux += char;
                        letterBuffer.push(char);
                    }
                }
                else {
                    buffAux += char;
                    letterBuffer.push(char);
                }
            } else if (isLeftParenthesis(char)) {
                if (buffAux.length) {
                    if (numberBuffer.length) {
                        buffAux = numberBuffer.join("") + buffAux;
                        numberBuffer = [];
                        result.push(new Token("Error, inicio de variables por letra no por numero", buffAux));
                        mens = " Error: inicio de Identificador por letra no por número. \n Línea "
                        return [result, mens]
                    }
                    else {
                        result.push(new Token("Identificador", buffAux));
                    }
                }
                letterBuffer = [];
                buffAux = "";
                emptyNumberBufferAsLiteral();
                result.push(new Token("Parentesis izquierdo", char));
            } else if (isRightParenthesis(char)) {
                if (buffAux.length) {
                    result.push(new Token("Identificador", buffAux));
                }
                letterBuffer = [];
                buffAux = "";
                emptyNumberBufferAsLiteral();
                result.push(new Token("Parentesis derecho", char));
            } else if (isComma(char)) {
                emptyNumberBufferAsLiteral();
                emptyLetterBufferAsVariables();
                result.push(new Token("Coma", char));
            } else if (isPC(char)) {
                emptyNumberBufferAsLiteral();
                emptyLetterBufferAsVariables();
                result.push(new Token("Punto y coma", char));
            } else if (isLeftKey(char)) {
                emptyNumberBufferAsLiteral();
                emptyLetterBufferAsVariables();
                result.push(new Token("Llave que abre", char));
            } else if (isRightKey(char)) {
                emptyNumberBufferAsLiteral();
                emptyLetterBufferAsVariables();
                result.push(new Token("Llave que cierra", char));
            } else {
                if (buffAux.length) {
                    if (numberBuffer.length) {
                        buffAux = numberBuffer.join("") + buffAux;
                        numberBuffer = [];
                        result.push(new Token("Error, inicio de variables por letra no por numero", buffAux));
                        mens = " Error: inicio de Identificador por letra no por número. "
                        return [result, mens]
                    }
                    else {
                        result.push(new Token("----", buffAux));
                    }
                }
                if (char !== ' ') {
                    result.push(new Token("Error", char));
                    mens = " Error: carácter "+ char +", no reconocido."
                    return [result, mens]
                }
            }
        }
        );
    compacta(result)
    return [result,mens]
    function compacta(result) {
        for (let i = 0; i > result.length; i++) {
            if (result[i] === undefined || result[i] === null) {
                result.splice(i, 1);
            }
        }
    }
    function emptyLetterBufferAsVariables() {
        if (numberBuffer.length) {
            result.push(new Token("Variable", letterBuffer.join("")));
            letterBuffer = [];
        }
    }
    function emptyNumberBufferAsLiteral() {
        if (numberBuffer.length) {
            result.push(new Token("Literal", numberBuffer.join("")));
            numberBuffer = [];
        }
    }
}
/**
 * 
 */
function ASLR(t, tokens, reglas_produccion, Edos_CC){
    let tok_aux = []
    i=-1
    tokens.forEach(
        function(lt, ind_){
            for(let x=0; x<lt.length; x++){
                switch(lt[x].type){
                    case "Identificador":{
                        tok_aux.push( {value: "Identificador", type:lt[x].type, ind_:ind_, name:lt[x].value} )
                        break
                    }
                    case "Funcion Booleana":{
                        tok_aux.push( {value: "Funcion_Booleana", type:lt[x].type, ind_:ind_, name:lt[x].value} )
                        break
                    }
                    case "Literal":{
                        tok_aux.push( {value: "Identificador", type:lt[x].type, ind_:ind_, name:lt[x].value} )
                        break
                    }
                    default:{
                        tok_aux.push( {value: lt[x].value, type:lt[x].type, ind_:ind_} )
                    }
                }
                tokMotor=JSON.parse(JSON.stringify(tok_aux[tok_aux.length-1]));
                if(tokMotor.value=="define"  || tokMotor.value=="principal"){
                    i++
                    motorTokens[i]=[]
                }
                if(tokMotor.value!=";" && tokMotor.value!="define" && tokMotor.value!=","){
                    if(tokMotor.value=="Identificador"){
                        tokMotor.value=tokMotor.name;
                        motorTokens[i].push(tokMotor)
                    }
                    else{
                        motorTokens[i].push(tokMotor)
                    }
                }
            }
        }
    )
    tok_aux.push( {value: "$", type:"Simbolo Terminal", ind_:tokens.length} )
    let Accion = t[0]
    let Ir_a = t[1]
    let pila = []
    let a, s
    let tok_actual = 0
    let seguir = false
    let mensaje = ""
    pila.push(0)
    do{
        s = pila[pila.length-1]
        a = tok_aux[ tok_actual ].value
        console.log( "\n\nToken: "+ tok_aux[ tok_actual ].value +", n: "+tok_actual+"\t\tAccion[ a:"+a+", S:"+ s+" ]:"+ Accion[a][s] )
        switch( typeof(Accion[a][s]) ){
            case 'string':{
                switch( Accion[a][s][0] ){
                    case "d":{
                        console.log( "Desplazar  " + Accion[a][s] )
                        let auxx = Accion[a][s].substr(1,Accion[a][s].length)
                        pila.push(a)
                        pila.push(auxx)
                        tok_actual++
                        seguir = true
                        break
                    }
                    case "r":{ //
                        console.log( "Reducir  "+ Accion[a][s]) 
                        let auxx = Accion[a][s].substr(1,Accion[a][s].length)
                        let pos_x_y_RP = auxx.split(",")
                        let RP = reglas_produccion[ pos_x_y_RP[0] ] 
                        RP = [ RP[0], [RP[1][pos_x_y_RP[1]]] ]
                        let jota
                        if(RP[1][0]!=="E"){
                            pila = pila.splice(0, pila.length-((RP[1][0].split(" ").length)*2))
                            jota = pila[pila.length-1]
                        }
                        else{
                            jota = pila[pila.length-1]
                        }
                        pila.push(RP[0])       
                        pila.push(Ir_a[ RP[0] ][jota])         
                        seguir = true
                        break
                    }
                    case "A":{
                        console.log("Aceptar")
                        err=Semantico();
                        if(err!="")
                            console.log(err);
                        mensaje = "Compilación exitosa"
                        return mensaje
                    }
                }
                break
            }
            default:{
                console.log("Error " + Accion[a] + "  "+ a + "  "+ s)
                seguir = false
                if( a == "$" ){
                    mensaje = "\nError: Desbalanceo, el método principal se encuentra desbalanceado."
                } 
                else 
                    if( tok_aux[ tok_actual ].value!=="Identificador" ) {
                        mensaje = "\n Error: Símbolo " + tok_aux[ tok_actual ].value + " inesperado.\n Línea " + (tok_aux[ tok_actual ].ind_+1) + " \n "
                    }
                    else{
                        mensaje = "\n Error: Símbolo " + tok_aux[ tok_actual ].name + " inesperado.\n Línea " + (tok_aux[ tok_actual ].ind_+1) + " \n "
                    }
                return mensaje
            }
        }
        console.log( "Pila: " + pila.toString() )
    }while( seguir )
}
function Semantico(){
    err=punteroSemantico(motorTokens.length-1);
    if(err!="")
        return err;
    for(let i=0;i<motorTokens.length-1;i++){
        for(j=i+1;j<motorTokens.length;j++)
            if(motorTokens[i][0].value==motorTokens[j][0].value)
                return "\nError: Doble definición de funcion " + motorTokens[i][0].value  +
                    " Línea: " + (motorTokens[i][0].ind_+1) + " y línea: "+(motorTokens[j][0].ind_+1);
        
    }
    funVisit.sort(sortNumber);
    for(let i=0;i<motorTokens.length;i++){
        if(i!=funVisit[i])
            return "\Advertencia: Función innecesaria" + motorTokens[i][0].value +
                " Línea: " + (motorTokens[i][0].ind_+1) ;
    }
    return "";
}
function sortNumber(a,b) {
    return a - b;
}
function punteroSemantico(indx,params=[]){
    funVisit.push(indx);
    let indIns=indx;
    let ins;
    params=params.map(Number);
    if(motorTokens[indIns][2].value==")" && params.length>0){
        return "-1";
    }
    if(motorTokens[indIns][2].value!=")" && params.length==0){
        return "-2" ;
    }
    if(params.length>0){
        varName=motorTokens[indIns][2].value;
        for( i=0;i<motorTokens[indIns].length;i++){
            if(motorTokens[indIns][i].value==varName)
                motorTokens[indIns][i].value=params[0];
        }
    }
    let error=true;
    for(let ins=1;ins<motorTokens[indIns].length;ins++){
        if(isNaN(motorTokens[indIns][ins].value)==true)
        if(PALABRAS_RESERVADAS.indexOf(motorTokens[indIns][ins].value)==-1 && 
            FUNCIONES_BOOLEANAS.indexOf(motorTokens[indIns][ins].value) ==-1 && 
            terminales.indexOf(motorTokens[indIns][ins].value)==-1){
                for(j=0;j<motorTokens.length;j++){
                    if(motorTokens[j][0].value==motorTokens[indIns][ins].value){
                        if(motorTokens[indIns][ins+2].value==")"){
                            err=punteroSemantico(j,[]);
                            if(err=="-2")
                                return "\nError: La función necesita un parámetro " + motorTokens[indIns][0].value  +
                                " Línea: " + (motorTokens[indIns][0].ind_+1) ;
                            if(err!="")
                                return err;
                        }
                        else{
                            err=punteroSemantico(j,[motorTokens[indIns][ins+2].value]);
                            if(err=="-1")
                                return "\nError: La función no recibe un parámetro " + motorTokens[indIns][ins].value  +
                                    " Línea: " + (motorTokens[indIns][ins].ind_+1) ;
                            if(err!=""){
                                return err;
                            }
                        }
                        error=false
                    }
                }
                if(error==true)
                    return "\nError: Función no declarada " + motorTokens[indIns][ins].value  +
                    " Línea: " + (motorTokens[indIns][ins].ind_+1);
        }
    }
    return "";
}
/**
 * @param {*} Edos_CC 
 * @param {*} S 
 * @param {*} reglas_produccion 
 */
function tablaAnalisis(Edos_CC, S, reglas_produccion) {
    terminales.splice(terminales.indexOf("E"), 1)
    let Accion = []
    for(let a = 0; a < terminales.length; a++)
        Accion[terminales[a]] = new Array(Edos_CC.length)
    let Ir_a = []
    for(let a = 0; a < no_terminales.length; a++)
        Ir_a[no_terminales[a]] = new Array(Edos_CC.length)
    for(let x = 0; x < Edos_CC.length; x++) {
        let aux_x = Edos_CC[x]
        aux_x.forEach(
            function (X) {
                let id_NT = X[0]
                let arr = X[1]
                for (let y = 0; y < arr.length; y++) {
                    if (compareObjects(arr[y], "·") == false) {
                        let aux_arr = arr[y].split(" ")
                        let pos = aux_arr.indexOf("·")
                        if (pos < aux_arr.length - 1) {
                            let simboloB = aux_arr[pos + 1]
                            let term = terminales.indexOf(simboloB) 
                            if ( term!==-1 ) { 
                                if (simboloB == "$") {  
                                    Accion[simboloB][x] = "Aceptar"
                                }
                                else{
                                    let temp = ir_a(arr[y], id_NT, CC)
                                    let n = getPosicionEdo(Edos_CC, temp)
                                    if (n !== -1) {
                                        Accion[simboloB][x] = "d" + n
                                    }
                                }
                            }
                            else {
                                if(no_terminales.indexOf(simboloB)!==-1){    
                                    let temp = ir_a(arr[y], id_NT, CC)
                                    let n = getPosicionEdo(Edos_CC, temp)
                                    if(n!==-1){
                                        Ir_a[simboloB][x] = "" + n
                                    }
                                }
                            }
                        }
                        else {
                            if (pos == aux_arr.length-1) {
                                let RP_ = arr[y].split(" ")
                                let RP_PP = RP_.indexOf("·")
                                RP_ = RP_.splice(0, RP_PP).join(" ")
                                let k = no_terminales.indexOf(id_NT)
                                let aux_RP_N = reglas_produccion[k][1].indexOf(RP_)
                                for (let a = 0; a < S[id_NT].length; a++) {
                                    if( typeof(Accion[S[id_NT][a]][x])=='undefined' ){
                                        Accion[S[id_NT][a]][x] = "r"+k+","+aux_RP_N
                                    }
                                }
                            }
                        }
                    }
                    else{
                        let RP_ = arr[y].split(" ")
                        let RP_PP = RP_.indexOf("·")
                        RP_ = RP_.splice(0, RP_PP).join(" ")
                        if(RP_ == ""){
                            RP_ = "E"
                        }
                        let k = no_terminales.indexOf(id_NT)
                        let aux_RP_N = reglas_produccion[k][1].indexOf(RP_)
                        for (let a = 0; a < S[id_NT].length; a++) {
                            if( typeof(Accion[S[id_NT][a]][x])=='undefined' ){
                                Accion[S[id_NT][a]][x] = "r"+k+","+aux_RP_N 
                            }
                        }
                    }
                }
            }
        )
    }
    return [Accion, Ir_a]
}
function getPosicionEdo(A, p) {
    for (let x = 0; x < A.length; x++) {
        if (compareObjects(A[x], p) ) {
            return x
        }
    }
    return -1
}
/**
 * Para el algoritmo de la colección canónica es necesaria la gramática G, 
 * además de generar la gramática aumentada G' tal que S, S' -> S
 * con S=Simbolo inicial de la gramática
 */
var CC = {}
function ColeccionCanonica(reglas_produccion_aumentada) {
    reglas_produccion_aumentada.forEach(
        function (X) {
            CC[X[0]] = new Array()
            for (var n = 0; n < X[1].length; n++) {
                if (X[1][n] !== "E") {
                    CC[X[0]][n] = "· " + X[1][n]
                }
                else {
                    CC[X[0]][n] = "·"
                }
            }
        }
    ) 
    let CERR_f_ = new Array()   
    CERR_f_.push(cerradura(["S'", CC["S'"]], CC))
    let cambio_d
    do {
        cambio_d = false
        for (let a = 0; a < CERR_f_.length; a++) {
            for (let b = 0; b < CERR_f_[a].length; b++) { 
                if (typeof (CERR_f_[a][b]) !== undefined) {  
                    for (let c = 0; c < CERR_f_[a][b][1].length; c++) {
                        let aux_p = CERR_f_[a][b][1][c].split(" ").indexOf("·")
                        let edo = []
                        if (aux_p < CERR_f_[a][b][1][c].split(" ").length - 1) {
                            edo = ir_a(CERR_f_[a][b][1][c], CERR_f_[a][b][0], CC)
                            let esta = compruebaEstado(CERR_f_, edo)
                            if (!esta) {
                                CERR_f_.push(edo)
                                cambio_d = true
                            }
                        }
                    }
                }
            }
        }
    } while (cambio_d); 
    return CERR_f_
} 
function compareObjects(obj1, obj2) {
    return JSON.stringify(obj1) === JSON.stringify(obj2);
}
function compruebaEstado(A, p){
    for (let x = 0; x < A.length; x++) {
        if (compareObjects(A[x], p)) { return true }
    }
    return false
}
function ir_a(I, X, CC) {
    let aux = I.toString().split(" ")
    posicion = aux.indexOf("·") 
    if (posicion < aux.length - 1) {
        let aux2 = aux[posicion]
        aux[posicion] = aux[posicion + 1]
        aux[posicion + 1] = aux2
        let naux = new Array()
        naux[0] = X
        naux[1] = [aux.join(" ")]
        if ((posicion + 1) == aux.length) {
            return []
        }
        else {
            return cerradura(naux, CC)
        }
    }
    return []
}
function cerradura(I, CC) {
    let J = new Array()
    J.push(I)
    let cambio = true
    do {
        cambio = false
        for (let a = 0; a < J.length; a++) {
            for (let b = 0; b < J[a][1].length; b++) {
                let aux2 = J[a][1][b]
                aux2 = aux2.toString()
                aux2 = aux2.split(" ")
                posicion = aux2.indexOf("·")
                let B = aux2[posicion + 1]
                if (no_terminales.indexOf(B) !== -1) {
                    let A_aux = new Array()
                    A_aux[0] = B
                    A_aux[1] = CC[B]
                    let esta = false
                    for (let nn = 0; nn < J.length; nn++) {
                        if (J[nn][0] == B) {
                            esta = true
                        }
                    }
                    if (esta == false) {
                        J.push(A_aux)
                        cambio = true
                    }
                }
            }
        }
    } while (cambio);
    return J
}
//
function Siguiente(reglas_produccion, primeros_) {
    let siguientes_ = []
    for (let i = 0; i < no_terminales.length; i++) {
        siguientes_[no_terminales[i]] = new Array()
    }
    siguientes_[no_terminales[0]].push("$")   
    let cambios_follows = true
    while (cambios_follows) {                 
        cambios_follows = false
        for (let i = 0; i < no_terminales.length - 1; i++) { 
            let auxNT = no_terminales[i] 
            for (let j = 0; j < reglas_produccion.length; j++){  
                let auxProd = reglas_produccion[j][1]
                for (let k = 0; k < auxProd.length; k++) {
                    let produccionItems = auxProd[k].split(" ")
                    let indiceNT = produccionItems.indexOf(auxNT) 
                    if (indiceNT != -1 && auxNT == produccionItems[produccionItems.length - 1]){ 
                        for (let sig_ = 0; sig_ < siguientes_[reglas_produccion[j][0]].length; sig_++) {
                            let aux_s_ = siguientes_[reglas_produccion[j][0]][sig_]
                            if (typeof (aux_s_) !== undefined && siguientes_[auxNT].indexOf(aux_s_) == -1) {
                                siguientes_[auxNT].push(aux_s_)
                                cambios_follows = true
                            }
                        }
                    } 
                    let posicion = no_terminales.indexOf(produccionItems[indiceNT + 1])
                    if (indiceNT != -1 && indiceNT < produccionItems.length) {
                        if (posicion == -1) { 
                            if (typeof (produccionItems[indiceNT + 1]) !== undefined && siguientes_[auxNT].indexOf(produccionItems[indiceNT + 1]) == -1) {
                                siguientes_[auxNT].push(produccionItems[indiceNT + 1])
                                cambios_follows = true
                            }
                        }
                        else { 
                            if (primeros_[no_terminales[posicion]].indexOf("E") == -1) {
                                for (let sig_ = 0; sig_ < primeros_[no_terminales[posicion]].length; sig_++) {
                                    if (typeof (primeros_[no_terminales[posicion]][sig_]) !== undefined && siguientes_[auxNT].indexOf(primeros_[no_terminales[posicion]][sig_]) == -1) {
                                        siguientes_[auxNT].push(primeros_[no_terminales[posicion]][sig_])
                                        cambios_follows = true
                                    }
                                }
                            }
                            else {
                                for (let sig_ = 0; sig_ < primeros_[no_terminales[posicion]].length; sig_++) { 
                                    if (siguientes_[auxNT].indexOf(primeros_[no_terminales[posicion]][sig_]) == -1 && primeros_[no_terminales[posicion]][sig_] != "E" && typeof (primeros_[no_terminales[posicion]][sig_]) !== undefined) {
                                        siguientes_[auxNT].push(primeros_[no_terminales[posicion]][sig_])
                                        cambios_follows = true
                                    }
                                } 
                                for (let sig_ = 0; sig_ < siguientes_[reglas_produccion[j][0]].length; sig_++) {
                                    let aux_s_ = siguientes_[reglas_produccion[j][0]][sig_]
                                    if (typeof (aux_s_) !== undefined && siguientes_[auxNT].indexOf(aux_s_) == -1 && aux_s_ != "E") {
                                        siguientes_[auxNT].push(aux_s_)
                                        cambios_follows = true
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    for (let np = 0; np < no_terminales.length; np++) {
        let pos_und = siguientes_[no_terminales[np]].indexOf(undefined)
        if (pos_und !== -1) {
            siguientes_[no_terminales[np]].splice(pos_und, 1)
        }
    }
    return siguientes_
}
function Primero(reglas_produccion) {
    let primeros_ = []
    for (let i = 0; i < no_terminales.length - 1; i++) {  
        let auxNT = no_terminales[i]    //
        primeros_[auxNT] = new Array()
        for (let k = 0; k < reglas_produccion[i][1].length; k++) {  
            let produccionItems = reglas_produccion[i][1][k].split(" ")
            let _terminal_ = false
            if (terminales.indexOf(produccionItems[0]) != -1 && typeof (produccionItems[0]) !== undefined && primeros_[auxNT].indexOf(produccionItems[0]) == -1) {
                primeros_[auxNT].push(produccionItems[0])
                _terminal_ = true
            }
            if (produccionItems.length == 1 && (produccionItems[0] == "E" || produccionItems[0] == "") && primeros_[auxNT].indexOf("E") == -1) {
                primeros_[auxNT].push(produccionItems["E"])
            }
            let ind_ = no_terminales.indexOf(produccionItems[0])
            let pilaAux = []
            let pilaVisitados = []
            pilaAux.push(auxNT)
            pilaVisitados.push(auxNT)
            if (!_terminal_ && ind_ != -1) {
                pilaAux.pop() 
                let prod_actual_ = reglas_produccion[ind_]  
                for (let aux_pi_ = 0; aux_pi_ < prod_actual_[1].length; aux_pi_++) {
                    let aux_item = prod_actual_[1][aux_pi_].split(" ")
                    aux_item = aux_item[0] 
                    if (pilaVisitados.indexOf(aux_item) == -1) {
                        pilaAux.push(aux_item)
                        pilaVisitados.push(aux_item)
                    }
                    do {
                        aux_item = pilaAux.pop()
                        if (terminales.indexOf(aux_item) != -1 && primeros_[auxNT].indexOf(aux_item) == -1 && typeof (aux_item) !== undefined) {
                            primeros_[auxNT].push(aux_item)
                        }
                        else { 
                            ind_ = no_terminales.indexOf(aux_item)
                            if (ind_ != -1) {
                                let aux1 = reglas_produccion[ind_]
                                for (let _x_ = 0; _x_ < aux1[1].length; _x_++) {
                                    let aux2 = aux1[1][_x_].split(" ")
                                    if (pilaVisitados.indexOf(aux2[0].toString()) == -1) {
                                        pilaAux.push(aux2[0])
                                        pilaVisitados.push(aux2[0])
                                    }
                                }
                            }
                            else {
                                if (terminales.indexOf(aux_item) != -1 && typeof (aux_item) !== undefined && primeros_[auxNT].indexOf(aux_item) == -1) {
                                    primeros_[auxNT].push(aux_item)
                                }
                            }
                        }
                    } while (pilaAux.length > 0)
                }
            }
        }
    }
    return primeros_
}
function getArrayReglasAumentadas(){
    let reglas_produccion = getArrayReglas()
    let reglas_produccion_aumentada = []
    reglas_produccion_aumentada[0] = new Array()
    reglas_produccion_aumentada[0][0] = "S'"
    reglas_produccion_aumentada[0][1] = new Array(no_terminales[1] + " $")
    for (let a = 0; a < reglas_produccion.length; a++) {
        reglas_produccion_aumentada[a + 1] = reglas_produccion[a]
    }
    return reglas_produccion_aumentada
}
function getArrayReglas() {
    let J = getReglas()
    let reglasMap = []
    for (let a = 0; a < J.length; a++) {
        reglasMap[a] = new Array()
        reglasMap[a][0] = J[a][0]
        reglasMap[a][1] = new Array()
        reglasMap[a][1] = J[a][1].split("|")
    }
    return reglasMap
}
function getReglas() {
    let X = [
        ["Declaracion_de_programa", "[Declaracion_de_metodo] principal ( ) Bloque"],
        ["[Declaracion_de_metodo]", "E|Declaracion_de_metodo"],
        ["Declaracion_de_metodo", "Tipo Firma_de_metodo Bloque"],
        ["Firma_de_metodo", "Identificador Parametro_opcional"],
        ["Parametro_opcional", "( [Identificador] )"],
        ["[Identificador]", "E|Identificador"],
        ["Tipo", "define"],
        ["Argumento_opcional", "( [Expresion_entera] )"],
        ["[Expresion_entera]", "E|Expresion_entera"],
        ["Bloque", "{ [Expresion] }|{ [Expresion] } [Declaracion_de_metodo]"], 
        ["[Expresion]", "Expresion|Instruccion"],
        ["Expresion", "Instruccion [Instruccion]|E"],
        ["[Instruccion]", "Instruccion Expresion|E"],
        ["Instruccion", "Bloque|Expresion_vacia|Expresion_Si|Expresion_Mientras|Expresion_Iterar|Expresion_Apagar|Expresion_girarALaIzquierda|Expresion_mover|Expresion_recogeZumbador|Expresion_ponZumbador|Expresion_retorno|Expresion_de_llamada"],
        ["Expresion_de_llamada", "Identificador Argumento_opcional ;"],
        ["Expresion_Si", "si ( Termino ) Bloque [sino]"],
        ["[sino]", "sino Bloque|E"],
        ["Expresion_Mientras", "mientras ( Termino ) Bloque"],
        ["[Termino]", "( Termino )"],
        ["Termino", "CY|CN|[Termino]"], 
        ["CY", "CN CO|[Termino] COO|E"],
        ["CO", "AND CY|OR CY|E"],
        ["COO", "AND CY|OR CY|E"],
        ["CN", "NOT Clausula_atomica|Clausula_atomica"],
        ["Clausula_atomica", "esCero ( Expresion_entera )|Funcion_Booleana"],
        ["Expresion_entera", "Identificador|predecesor Argumento_opcional|sucesor Argumento_opcional"],
        ["Expresion_Iterar", "iterar ( Expresion_entera ) [Expresion]"],
        ["Expresion_Apagar", "apagar Argumento_vacio ;"],
        ["Expresion_girarALaIzquierda", "girarALaIzquierda Argumento_vacio ;"],
        ["Expresion_mover", "mover Argumento_vacio ;"],
        ["Expresion_recogeZumbador", "recogeZumbador Argumento_vacio ;"],
        ["Expresion_ponZumbador", "ponZumbador Argumento_vacio ;"],
        ["Expresion_retorno","retornar Argumento_vacio ;"],
        ["Argumento_vacio", "( )"],
        ["Expresion_vacia", ";"] 
    ]
    return X
}
