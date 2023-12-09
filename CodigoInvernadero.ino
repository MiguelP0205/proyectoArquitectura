/**
*Fecha:2023-12-08
*@autor Valeria Bastidas Torres <valeriabastidas@unicauca.edu.co>
*@autor Laura Sofia Botina Montero <laubotina@unicauca.edu.co>
*@autor Miguel Angel Polo Gómez <mpolo@unicauca.edu.co>

@details: Este proyecto consiste en el control y monitoreo de un invernadero inteligente a traves de la medición de sensores de temperatura, humedad y luz**/

#include <Keypad.h>
#include <LiquidCrystal.h>
#include "AsyncTaskLib.h"
#include "StateMachineLib.h"
#include "DHT.h" 

/**
*@brief Asignacion de Pines para el led RGB
**/
#define LED_RED 48
#define LED_GREEN 46
#define LED_BLUE 44
/**
*@brief Asignacion de Pines para el fotoresistor y el sensor DHT11
**/
#define PIN_LUZ A0
#define PIN_TEMYHUM A1
#define DHTTYPE DHT11
DHT dht(PIN_TEMYHUM, DHTTYPE);
/**
*@brief Asignacion de Pines para el buzzer
**/
#define BUZZER 7
/**
*@brief Asignacion de Pines de keypad
*Configuracio de filas y columnas del KeyPad
**/
const byte ROWS = 4; 
const byte COLS = 3; 
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {24, 26, 28, 30}; 
byte colPins[COLS] = {32, 34, 36}; 
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
/**
*@brief Asignacion de Pines para la pantalla LCD
**/
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

/**
*@brief Asignacion de Variables
**/
boolean estado;
String password= "1234";
String passIngresada;
int count = 0;
int numIntentos=0;

/**
*@brief Asignacion nota musicales y frecuencias para el buzzer
**/
#define NOTE_B0  31  
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978


/**
*@brief Estados de la maquina de estados
*INIT Estado inicial de la maquina
*monitoreoTH Estado donde se monitorea la temperatura y humedad
*monitoreoL Estado donde se monitorea la luz
*alarma Estado donde se activa la alarma
*bloqueado Estado donde se bloquea el sistema
**/
enum State
{
  INIT= 0,
  monitoreoTH = 1,
  monitoreoL= 2,
  alarma= 3,      
  bloqueado=4
};

/**
*@brief Transiciones de la maquina de estados
*sigEstNoCond Permite avanzar al siguiente estado cuando no sobrepasó la condición
*sigEstCond Permite avanzar al siguiente estado cuando sobrepasó la condición
*regEstNoCond Permite regresar al estado del que fue origen donde no se cumplió la condición  
*regEstCond Permite regresar al estado del que fue origen donde se cumplió la condición
*desc Evita transiciones de Estado no Controladas
**/
enum Input  
{
  sigEstNoCond = 0, 
  sigEstCond = 1, 
  regEstNoCond = 2, 
  regEstCond = 3,
  desc = 4 
};
/**
*@brief Máquina de estados
*@param estados
*@param transiciones
**/
StateMachine stateMachine(5,8);

/**@brief Guarda el ultimo input ingresado por el usuario*/
  Input currentInput;

/**@brief Declaracion de funciones */
void seguridad();
void temperatura();
void luz();
void actAlarma();

/**@brief Establece el valor de 'currentInput' al estado 'regEstNoCond' después de un temporizador de 5 segundos*/
void tmOut5s(){
  currentInput = regEstNoCond;
}
/**@brief Establece el valor de 'currentInput' al estado 'sigEstNoCond' después de un temporizador de 5 segundos*/
void tmOut5s2(){
  currentInput = sigEstNoCond;
}
/**@brief Establece el valor de 'currentInput' al estado 'regEstCond' después de un temporizador de 6 segundos*/
void tmOut6s(){
  currentInput = regEstCond;
}
/**@brief Establece el valor de 'currentInput' al estado 'regEstNoCond' después de un temporizador de 3 segundos.*/
void tmOut3s(){
  currentInput = regEstNoCond;
}
/**
*@brief Tareas asincronicas
*asyncTaskSeguridad  Tarea encargada del ingreso al sistema
*asyncTask5s  Tarea encargada del Timeout de 5 segundos entre los estados de bloqueo e Init
*asyncTaskTemperatura  Tarea encargada del monitoreo de la temperatura y humedad
*asyncTask5s2  Tarea encargada del Timeout 5 segundos del estado de monitoreo de temperatura-humedad
*asyncTaskLuz  Tarea encargada del monitoreo de la luz
*asyncTask6s  Tarea encargada del Timeout 6 segundos entre los estados de alarma y monitoreo de temperatura y humedad
*asyncTaskAlarma  Tarea encargada de la activación de la alarma
*asyncTask3s  Tarea encargada del Timeout 3 segundos del estado de monitor luz 
**/
/*details: las tarea asincronicas que tienen la palabra 'true' despues del tiempo porque estas tareas se ejecutan más de una vez*/

AsyncTask asyncTaskSeguridad(1000,seguridad);
AsyncTask asyncTask5s(5000,tmOut5s);
AsyncTask asyncTaskTemperatura(1000, true, temperatura);
AsyncTask asyncTask5s2(5000,tmOut5s2);
AsyncTask asyncTaskLuz(1000, true, luz);
AsyncTask asyncTask6s(6000,tmOut6s);
AsyncTask asyncTaskAlarma(1000, actAlarma);
AsyncTask asyncTask3s(3000,tmOut3s);

/**
*@brief Congigura la máquina de estados
**/
void setupStateMachine()
{
  /**
  *@brief Se agregan transiciones entre los estados de la máquina de estados
  **/
  stateMachine.AddTransition(INIT, monitoreoTH, []() { return currentInput == sigEstCond; });
   stateMachine.AddTransition(INIT, bloqueado, []() { return currentInput == sigEstNoCond; });
   stateMachine.AddTransition(bloqueado, INIT, []() { return currentInput == regEstNoCond; });
   stateMachine.AddTransition(monitoreoTH, alarma, []() { return currentInput == sigEstCond;});

   stateMachine.AddTransition(monitoreoTH, monitoreoL, []() { return currentInput == sigEstNoCond; });
   stateMachine.AddTransition(monitoreoL, monitoreoTH, []() { return currentInput == regEstNoCond; });
   stateMachine.AddTransition(monitoreoL, alarma, []() { return currentInput == sigEstCond; });
   stateMachine.AddTransition(alarma, monitoreoTH, []() { return currentInput == regEstCond; });
  
  /**
  *@brief Se configura las acciones a realizar al entrar a cada estado
  **/
   stateMachine.SetOnEntering(INIT, input_init);
   stateMachine.SetOnEntering(bloqueado, input_bloqueado);
   stateMachine.SetOnEntering(monitoreoTH, input_monitoreoTH);
   stateMachine.SetOnEntering(monitoreoL, input_monitoreoL);
   stateMachine.SetOnEntering(alarma, input_alarma);
  
  /**
  *@brief Se configura las acciones a realizar al salir de cada estado
  **/
   stateMachine.SetOnLeaving(INIT, output_init);
   stateMachine.SetOnLeaving(bloqueado, output_bloqueado);
   stateMachine.SetOnLeaving(monitoreoTH, output_monitoreoTH);
   stateMachine.SetOnLeaving(monitoreoL, output_monitoreoL);
   stateMachine.SetOnLeaving(alarma, output_alarma);
}
/**
*@brief Configura el estado inicial de la máquina de estados
*Inicia la tarea de seguridad
**/
void input_init()
{
  currentInput=static_cast<Input>(Input::desc);
  asyncTaskSeguridad.Start();
}
/**@brief Realiza acciones al salir del estado inicial
*Se detiene la tarea de seguridad
*/
void output_init()
{
  asyncTaskSeguridad.Stop();    
}
/**@brief Configura el estado de bloqueo de la máquina de estDado
* LED_RED Esta configurado en encenderse cuando esta en estado de bloqueo
*/
void input_bloqueado()
{
  currentInput=static_cast<Input>(Input::desc);
  asyncTask5s.Start(); 
  digitalWrite(LED_RED, HIGH);
  lcd.print("Sist. Bloqueado");
}
/**@brief Realiza acciones al salir del estado de bloqueo
*LED_RED El led se apaga al salir del estado de bloqueo
*count se inicializa en cero la variable
*/
void output_bloqueado()
{
  count = 0;
  digitalWrite(LED_RED, LOW);
  lcd.clear();
  asyncTask5s.Stop();
}
/**@brief Configura el estado de monitoreo de temperatura y humedad 
*Inicia la tarea asíncronica de monitoreo de temperatura y humedad
*Inicia la tarea Timeout (medicion del tiempo) de 5 segundos
*/
void input_monitoreoTH()
{
  currentInput=static_cast<Input>(Input::desc);
  digitalWrite(LED_GREEN, LOW);
  asyncTaskTemperatura.Start();
  asyncTask5s2.Start();
}
/**@brief Acciones que realiza al salir del estado de monitoreo de humedad y temperatura
*Se detiene la tarea asíncronica de humedad y temperatura
*Se detiene la tarea Timeout (medicion del tiempo) de 5 segundos
*/
void output_monitoreoTH()
{
  asyncTaskTemperatura.Stop();
  asyncTask5s2.Stop();
}
/**@brief Configura el estado de monitoreo de luz 
*Inicia la tarea asíncronica de monitoreo de luz
*Inicia la tarea Timeout (medicion del tiempo) de 3 segundos
*/
void input_monitoreoL()
{
  currentInput=static_cast<Input>(Input::desc);
  lcd.clear();
  asyncTaskLuz.Start();
  asyncTask3s.Start();
}
/**@brief Acciones que realiza al salir del estado de monitoreo de luz
*Detiene la tarea asíncronica de monitoreo de luz
*Detiene la tarea Timeout (medicion del tiempo) de 3 segundos
*/
void output_monitoreoL()
{
  asyncTaskLuz.Stop();
  asyncTask3s.Stop();
}
/**@brief Configura el estado de alarma
*Inicia la tarea asíncronica de alarma
*Inicia la tarea asincronica Timeout (medicion del tiempo) de 6 segundos
*/
void input_alarma()
{
  currentInput=static_cast<Input>(Input::desc);
  lcd.clear();
  asyncTask6s.Start();
  asyncTaskAlarma.Start();
}
/**@brief Acciones que realiza al salir del estado de alarma
 *Detiene la tarea asíncronica de alarma
 *Detiene la tarea asincronica Timeout (medicion del tiempo) de 6 segundos
 */
void output_alarma()
{
  asyncTaskAlarma.Stop();
  asyncTask6s.Stop();
  noTone(BUZZER);
}
void setup() {
  /**@brief Comunicacion con el puerto serial*/
  Serial.begin(9600);
  
  /**@brief Configura los pines del led y el buzzer como salidas*/
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUZZER,OUTPUT);

  /**@brief Inicialización de la pantalla Lcd
   * Imprime un mensaje por pantalla
   */
  lcd.begin(16, 2);
  lcd.print("Bienvenido");
  
  /**@brief Inicia el sensor DHT*/
  dht.begin();
  delay(2000);

  /**@brief Inicia la máquina de estados*/
  setupStateMachine();  
  stateMachine.SetState(State::INIT, false, true);

}

void loop() 
{
  /**@brief Actualizacion de las tareas asincronicas*/
  asyncTaskSeguridad.Update();
  asyncTaskTemperatura.Update();
  asyncTaskLuz.Update();
  asyncTask5s.Update();
  asyncTask5s2.Update();
  asyncTask6s.Update();
  asyncTaskAlarma.Update();
  asyncTask3s.Update();
  
  /**@brief Actualiza la máquina de estados*/
  stateMachine.Update();
 
}
/*
*@brief Solicita la usuario que ingrese una clave y verifica si es correcta
*/
void  seguridad(){
  do{
    lcd.setCursor(0,0);
    
    lcd.print("Ingresar clave:");
    lcd.setCursor(0,1);
    do{
      char key = keypad.getKey();
      if (key){
        lcd.print('*');
        passIngresada += key;
      }
    }while(passIngresada.length() != password.length());

    if (passIngresada.equals(password)) {
      lcd.clear();
      lcd.print("Clave correcta");
      digitalWrite(LED_GREEN, HIGH);  
      digitalWrite(LED_RED, LOW); 
      digitalWrite(LED_BLUE, LOW);
      delay(1200);                     
      lcd.clear();
      passIngresada = "";
      count =0;
      currentInput=static_cast<Input>(Input:: sigEstCond);
      break;
    } else {
      lcd.clear();
      lcd.print("Clave incorrecta");
      digitalWrite(LED_BLUE,HIGH);
      digitalWrite(LED_GREEN, LOW);  
      digitalWrite(LED_RED, LOW);
      delay(1000);
      digitalWrite(LED_BLUE, LOW);
      lcd.clear();
      passIngresada = "";
      count += 1;
    }
  }while(count != 3);   
  digitalWrite(LED_BLUE,LOW);
  digitalWrite(LED_GREEN, LOW);
  lcd.clear();
  if(count == 3)
  {
    currentInput=static_cast<Input>(Input:: sigEstNoCond);
  }
}

/**
 * @brief Lee la temperatura y humedad del sensor DHT11 e imprime los resultados en la pantalla LCD
*/
void temperatura(){
  lcd.clear();
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();
  float f = dht.readTemperature(true);


  if (isnan(hum) || isnan(temp) || isnan(f)) {
     Serial.println(F("Failed to read from DHT sensor!"));
   return;
 }

  float hif = dht.computeHeatIndex(f, hum);
  float hic = dht.computeHeatIndex(temp, hum, false);


  lcd.setCursor(0,0);
  lcd.print("Humedad: ");
  lcd.print(hum);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Tem:"); 
  lcd.print(temp);
  lcd.print("C");

  if(temp > 30 && hum > 70){
    currentInput=static_cast<Input>(Input:: sigEstCond);
  }
}
/**
 * @brief Lee el valor de luz del fotoresistor y lo muestra en la pantalla LCD
*/
void luz(){
  lcd.clear();
  lcd.setCursor(0,0);
  int valLuz = analogRead(PIN_LUZ);
  lcd.print("Valor luz: ");
  
  lcd.print(valLuz);
  
  if(valLuz < 20){
    currentInput=static_cast<Input>(Input:: sigEstCond);
  }
}
/**
 * @brief Reproduce una alarma utilizando el buzzer y muestra un mensaje en la pantalla LCD
 */

 void actAlarma(){
  lcd.setCursor(0,0);
  lcd.print("Limite");
  lcd.setCursor(0,1);
  lcd.print("Sobrepasado!");

  tone(BUZZER, NOTE_C4);
  delay(500);
 
  tone(BUZZER, NOTE_D4);
  delay(500);

  tone(BUZZER, NOTE_E4);
  delay(500);

  tone(BUZZER, NOTE_F4);
  delay(500);
 
  tone(BUZZER, NOTE_G4);
  delay(500);

  tone(BUZZER, NOTE_A4);
  delay(500);

  tone(BUZZER, NOTE_B4);
  delay(500);

  tone(BUZZER, NOTE_C5);
  delay(500);

  tone(BUZZER, NOTE_C6);
}
