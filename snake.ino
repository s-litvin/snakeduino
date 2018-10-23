// Тестировалось на Arduino IDE 1.0.1
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>


#define RESOLUTION 4
#define MAXLEN 100
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

#define DISP_HEIGHT 84
#define DISP_WIDTH 48

#define BUTTON 8


char direction = RIGHT;
bool game_over = false;

char sensorPinX = A0;
char sensorPinY = A1;
char sensorValueX = 0;
char sensorValueY = 0;
char snakesize = 6;

char snakex[MAXLEN];
char snakey[MAXLEN];

char foodx = -1;
char foody = -1;

// Nokia 5110 LCD
// pin 3 - Serial clock out (SCLK)
// pin 4 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 6 - LCD chip select (CS)
// pin 7 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(3, 4, 5, 6, 7);


void setup() {
  pinMode(BUTTON, INPUT_PULLUP);
  
  display.begin();              // Инициализация дисплея
  display.setContrast(60);      // Устанавливаем контраст
  //display.setTextColor(WHITE, BLACK);  // Устанавливаем цвет текста
  display.setTextColor(BLACK, WHITE);  // Устанавливаем цвет текста
  display.setTextSize(1);       // Устанавливаем размер текста
  display.clearDisplay();       // Очищаем дисплей
  display.display();
  delay(200); 
  display.fillRect(0, 0, 84, 46, BLACK);
  display.setCursor(20,10);
  display.println("SNAKE");
  display.println("Game");
  
  display.display();
  delay(1000);

  init_snake();
}

void loop() 
{
  if (game_over) {
    display.clearDisplay();
    display.display();
    show_score();
    while(digitalRead(BUTTON)) {
      // while not pressed button, do nothing
    }
    display.clearDisplay();
    display.display();
    game_over = false;
    snakesize = 6;
    init_snake();
  }
  
  display.clearDisplay();
  movesnake();
  food();
  snake();
 
  //display.setTextColor(BLACK);
  //display.setTextSize(0.5);
  //display.setCursor(0, 0);
  //display.println(snakesize);
  //display.println(foody);
  //display.println(snakex[0]);
  //display.println(snakey[0]);
  display.display();

  // if button is pressed, time speed up
  if (digitalRead(BUTTON) == LOW) {
    delay(100);
  } else {
    delay(350);
  }
}

static void init_snake()
{
  for (char i = 0; i < MAXLEN; i++){
     snakex[i] = -2;
     snakey[i] = -2;
  }

  for (char i = 0; i < snakesize; i++){
     snakex[i] = 11-i;     
     snakey[i] = 5;     
  }

  direction = RIGHT;

  for(char i=0; i <= 3; i++) {
    snake();
    display.display();
    delay(350);
    display.clearDisplay();
    display.display();
    delay(350);
  }
}

static void show_score()
{
  display.setCursor(25, 10);
  display.setTextSize(1);
  int score = snakesize;
  display.print("SCORE: ");
  display.println(score);
  display.println(" ");
  display.print("press button");
  display.display();
}


static void food ()
{
  if (foodx < 0) {
      foodx = random(0, DISP_HEIGHT / RESOLUTION);
      foody = random(0, DISP_WIDTH / RESOLUTION);
  } 
  display.drawCircle(foodx * RESOLUTION, foody * RESOLUTION, RESOLUTION / 2, BLACK); 
}

static void snake () 
{

    for (char i = 0; i < MAXLEN; i++) {
      if (snakex[i] == -2) {
        break;
      }
      
      if (i == 0) {
        display.drawCircle(snakex[i] * RESOLUTION, snakey[i] * RESOLUTION, RESOLUTION / 2, BLACK);  
      } else {
        
        if (snakex[0] == snakex[i] && snakey[0]==snakey[i]) {
          game_over = true;
        }
        
        // filled body while not pressed. Low - btn not pressed. High - pressed.
        if (digitalRead(BUTTON) == LOW) {
           display.drawCircle(snakex[i] * RESOLUTION, snakey[i] * RESOLUTION, RESOLUTION / 2, BLACK); 
          //display.fillRect(snakex[i] * RESOLUTION, snakey[i] * RESOLUTION, RESOLUTION, RESOLUTION, BLACK);  
        } else {
          display.fillCircle(snakex[i] * RESOLUTION, snakey[i] * RESOLUTION, RESOLUTION / 2, BLACK); 
        }
      }
    }
}

static void movesnake () 
{
    char tmpx = snakex[0];
    char tmpy = snakey[0];

    char xdirection = getDirection(1); // 1 - x
    char ydirection = getDirection(2); // 2 - y

    if (xdirection != 0) {
      if (xdirection > 0 && direction != LEFT) {
          direction = RIGHT;
      } else if (xdirection < 0 && direction != RIGHT) {
          direction = LEFT;
      }
    } else if (ydirection != 0) {
      if (ydirection > 0 && direction != DOWN) {
          direction = UP;
      } else if (ydirection < 0 && direction != UP) {
          direction = DOWN;
      }
    }

    switch (direction) {
      case UP:
        xdirection = 0; ydirection = -1;
        break;
      case DOWN:
        xdirection = 0; ydirection = 1;
        break;
      case LEFT:
        xdirection = -1; ydirection = 0;
        break;
      case RIGHT:
        xdirection = 1; ydirection = 0;
        break;
    }
    
    char prevx = tmpx + xdirection;
    char prevy = tmpy + ydirection;

    if (prevx >= DISP_HEIGHT / RESOLUTION) {
      prevx = 0;
    } else if (prevx == -1) {
      prevx = DISP_HEIGHT / RESOLUTION;
    }
    if (prevy >= DISP_WIDTH / RESOLUTION) {
      prevy = 0;
    } else if (prevy == -1) {
      prevy = DISP_WIDTH / RESOLUTION;
    }
    
    for (char i = 0; i < MAXLEN; i++) {
      if (i == 0) {
        if (foodx == snakex[i] && foody == snakey[i]) {
          if (snakesize < MAXLEN) {
            snakex[snakesize] = 0;
            snakey[snakesize] = 0;
            snakesize++;
          }
          
          foodx = -1;
        }
      }
      if (snakex[i] == -2 || (prevx == snakex[i] && prevy == snakey[i])) {
        break;
      }
      tmpx = snakex[i];
      tmpy = snakey[i];
      
      snakex[i] = prevx;
      snakey[i] = prevy;      

      prevx = tmpx;
      prevy = tmpy;
     }
}

static char getDirection(char ax)
{
    char sensorPin = sensorPinY;
    if (ax == 1) {
      sensorPin = sensorPinX;
    }
    return map(analogRead(sensorPin), 0, 1024, -1, 2);
}



