#include <Arduino.h>
#include <SoftwareSerial.h>

String SMSrecebida; // Variável global que recebe a SMS recebida pelo modulo.
String numero;      // Variável global que recebe o NUMERO de quem enviou o SMS.
String MSG;         // Variável global que recebe a MENSAGEM de quem enviou o SMS.
String data;        // Variável global que recebe a DATA de quem enviou o SMS.

bool SMSread();                                                // Retorna True se chegou algum dado pela Serial ou seja Chegou SMS no modulo
void Comandos_AT();                                            // Permite comunicação Serial do Arduino com o Modulo atraves do Monitor Serial
void Tornar_Legivel(String dado);                              // Retira os caracteres "Espaço" da string dado.
void enviarSMS(String telefone, String data, String mensagem); // Função que envia SMS.
void Resetar_modem();                                          // Função que reseta o modem.
void Numero_NaoCadastrado();                                   // Função que avisa ao número MASTER quem tentou resetar o equipamento.
void Limpar_CHIP();                                            // Limpa memoria do CHIP utilizado no módulo
String Retorna_numero(String SMS);                             // Recebe a string completa do modulo e retorna somente o NUMERO de quem enviou a SMS.
String Retorna_msg(String SMS);                                // Recebe a string completa do modulo e retorna somente o MENSAGEM de quem enviou a SMS.
String Retorna_data(String SMS);                               // Recebe a string completa do modulo e retorna somente a DATA de quem enviou a SMS.

void setup() // Configurações Iniciais
{
  Serial.begin(9600); // Inicializa Porta serial nativa do Arduino
  while (!Serial)
    ; // Aguarda comunicação serial ser estabelecida
  //Serial800l.begin(9600);  //  Inicializa a comunicação serial "virtual"
  pinMode(3, OUTPUT); // Pino 3 configurado como SAÍDA digital
}

void loop() // Loop eterno
{
  if (SMSread())
  {
    numero = Retorna_numero(SMSrecebida);
    MSG = Retorna_msg(SMSrecebida);
    data = Retorna_data(SMSrecebida);

    Tornar_Legivel(MSG);
    delay(10);
    if ((MSG == "RESETAR" || MSG == "LIMPAR") && (numero == "992647785" || numero == "953884989"))
    {
      if (MSG == "RESETAR")
      {
        Resetar_modem();
      }
      else if (MSG == "LIMPAR")
      {
        Limpar_CHIP();
      }
    }
    else if (MSG == "RESETAR")
    {
      Numero_NaoCadastrado();
    }
  }
  //else// "SE PRECISAR DEBUGAR O MODULO COM COMANDOS AT HABILITAR ESTE ELSE !"
  //{
  //  Comandos_AT();
  // }
}
void Numero_NaoCadastrado()
{
  String mensagem = "O numero: " + numero + " TENTOU resetar o modem NET AEROPORTO em: " + data;
  enviarSMS("992647785", data, mensagem);
  delay(10000);
  mensagem = "Voce nao tem permissao para me resetar !";
  enviarSMS(numero, data, mensagem);
}
void Limpar_CHIP()
{
  String del = "DEL ALL";
  Serial.println("AT+CMGDA = " + del + ""); //Comando utilizado para limpar dados armazenados
  delay(1000);
  String mensagem = "As mensagens armazenada no cartão SIM foram apagadas em: " + data;
  enviarSMS("992647785", data, mensagem);
}
void Resetar_modem()
{
  String mensagem = "O numero: " + numero + " resetou o modem NET AEROPORTO em: " + data;
  digitalWrite(3, !digitalRead(3));
  delay(10000);
  digitalWrite(3, !digitalRead(3));
  //Serial.println("O numero "+numero+" me resetou! em: " + data); DEBUGAR STRINGS NO CONSOLE SERIAL ARDUINO
  enviarSMS("992647785", data, mensagem);
}
void enviarSMS(String telefone, String data, String mensagem)
{
  Serial.print("AT+CMGS=\"" + telefone + "\"\n"); // comando necessario para enviar SMS
  Serial.println(mensagem);                       // Após \n escrevemos a mensagem
  Serial.print((char)26);                         // 26 TABELA ASCII = CTRL+Z // control+z indica o fim.
  delay(10);
  Serial.print((char)27); // ESQ TABELA ASCII
}
String Retorna_data(String SMS)
{
  String dado = SMS.substring(27, 44); // separa somente a DATA da informação recebida pelo modulo
  dado.trim();

  String ano = dado.substring(0, 2); // separa o ANO
  String dia = dado.substring(6, 8); // separa o DIA
  String mes = dado.substring(3, 5); // separa o MES

  dado.replace(ano + "/" + mes + "/" + dia, dia + "/" + mes + "/" + ano); // formata do formato ANO/MES/DIA PARA DIA/MES/ANO

  return dado; // retorna a data formatada
}
String Retorna_numero(String SMS)
{
  return SMS.substring(12, 21); // retorna o numero que esta contido na posição 12 até a 20 da string SMS.
}
void Tornar_Legivel(String dado)
{
  dado.trim();
  dado.toUpperCase();
  MSG = dado;
}
bool SMSread()
{
  if (Serial.available() > 0)
  {
    SMSrecebida = Serial.readString();
    SMSrecebida.trim();
    delay(10);
    //Serial.println(SMSrecebida); HABILITAR SE QUISER VER A SMS FULL RECEBIDA
    return true;
  }
  else
  {
    return false;
  }
}
String Retorna_msg(String SMS)
{
  return SMS.substring(49); // Apartir das posição 49 tudo é mensagem.
}
void Comandos_AT()
{
  if (Serial.available()) // Se eu enviei dado pelo monitor Serial
  {
    String dado = Serial.readString(); // armazena esse dado
    dado.trim();                       // retira os espaços desncessarios
    delay(30);
    Serial.println(dado); // escreve o dado no modulo
    delay(30);
    if (Serial.available() > 0) // se o modulo responder
    {
      dado = Serial.readString(); // armazeno a resposta
      dado.trim();
      Serial.println(dado); // escrevo no monitor serial
    }
  }
  else if (Serial.available()) // testar a SMS recebida
  {
    SMSrecebida = Serial.readString();
    SMSrecebida.trim();
    delay(10);
    Serial.println(SMSrecebida);
  }
}