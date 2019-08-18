#include <Servo.h>

const int servoPin = 5;

Servo servo;

// motor pins
const int stepPinX = 9; // обьявление 1-го шагового двигателя
const int dirPinX = 8;

const int stepPinY = 11; // обьявление 2-го шаговика
const int dirPinY = 10;

int currX = 0; // текущее значение Х
int currY = 0; // текущее значение У
int currZ = 0; // // текущее значение Z

void motorStep(int stepPin, int dirPin, bool forward) // функция совершения шага
{
  const int pulseDelay = 500; //величина задержки
  digitalWrite(dirPin, forward ? HIGH : LOW); // определяем направление вращеения
  digitalWrite(stepPin, HIGH);//подаем импульс шага
  delayMicroseconds(pulseDelay);//задержка на шаг
  digitalWrite(stepPin, LOW);//убираем импульс шага
  delayMicroseconds(pulseDelay);//задержка после импульса
  if (stepPin == stepPinX )//если работал шаговик Х
  {
    currX += forward ? 1 : -1;//меняем текущий х

  }
  else // если работал шаговик У
  {
    currY += forward ? 1 : -1; // меняем текущее знач У
  }
//Serial.print(currX);
//Serial.print(" ");
//Serial.println(currY);
}

void motorInit(int stepPin, int dirPin) // инициализация двигателя
{
  pinMode(stepPin, OUTPUT); // настраиваем пин степ на выход
  pinMode(dirPin, OUTPUT); //настраиваем дир пин на выход
}


void drawLine(int x1, int y1, int x2, int y2) // ф-ия рисования линии
{
  if ((currX == x1) && (currY == y1)) // проверка текущих координат относительно начальных
  {
    setZ(0); // опускаем перо
    moveTo(x2, y2); // передвигаемся в конечные координаты
  }
  else
  {
    setZ(1); // поднимаем перо
    moveTo(x1, y1); // передвигаемся в начальные координаты
    setZ(0);// опускаем перо
    moveTo(x2, y2);// передвигаемся в конечные координаты
  }
}



void drawArc(int x, int y, int I, int J, bool dir)
{
  setZ(0);
  // cx, cy - центр
  // x, y - конечные координаты
  // dx, dy - I, J
  float cx = currX + I;
  float cy = currY + J;
  float radius = sqrt(I * I + J * J);

  // найти развертку дуги
  float angle1 = atan2f(J, I) - M_PI; //абсолютное значение угла
  float angle2 = atan2f(y - cy, x - cx);
  float sweep = angle2 - angle1;

  if (dir && sweep < 0)
    angle2 += 2 * M_PI;
  else if (!dir && sweep > 0)
    angle1 += 2 * M_PI;

  sweep = angle2 - angle1;

  float len = abs(sweep) * radius;

  int num_segments = floor(len);

  for (int i = 0; i < num_segments; ++i)
  {
    // interpolate around the arc
    float fraction = ((float)i) / ((float)num_segments);
    float angle3 = ( sweep * fraction ) + angle1;

    // find the intermediate position
    float nx = cx + cos(angle3) * radius;
    float ny = cy + sin(angle3) * radius;
    // make a line to that intermediate position
    moveTo(nx, ny);
  }
}



void moveTo(int x, int y) // ф-ия передвижения
{
  int d = currZ == 1 ? 12 : 2; // установка величины задержки

  const int deltaX = abs(x - currX);// определение модуля разности текущего и заданного положения
  const int deltaY = abs(y - currY);
  const int signX = currX < x ? 1 : -1; // определение знака направления движения по Х
  const int signY = currY < y ? 1 : -1; // определение знака направления движения по У
  int error = deltaX - deltaY; // разность модулей
  while (currX != x || currY != y) // условия очередности срабатывания двигателей в зависимости от модульной ошибки
  {
    int error2 = error * 2;
    if (error2 > -deltaY)
    {
      error -= deltaY;
      motorStep(stepPinX, dirPinX, signX == 1);
      delay(d);
    }
    if ( error2 < deltaX)
    {
      error += deltaX;
      motorStep(stepPinY, dirPinY, signY == 1);
      delay(d);
    }
  }
}


void setZ(int z) // функция изменения положения пера
{
  if (z == 0)
  {
    servo.write(88); // опускание пера
    currZ = 1; // запоминание положения
  }
  else {
    servo.write(0); // подъем пера
    currZ = 0; // запоминание положения
  }
  delay(100); // задержка в 100 мс

}

void setXY0()// ф-ия возврата в нулевую точку
{

  int p = 0; // обьявление и обнуление переменной

  do {
    p = digitalRead(2); // присвоение значения 2-го пина (Х)
    motorStep(stepPinX, dirPinX, false); // делаем шаг назад по Х
  }
  while (p == HIGH); // пока концевик не замкнут


  currX = 0; // обнуление текущего положения Х

  do {
    p = digitalRead(3); // присвоение значения 3-го пина
    motorStep(stepPinY, dirPinY, false);// делаем шаг назад по У
  }
  while (p == HIGH);//пока концевик не замкнут
  currY = 0;// обнуление текущего положения У

}

void setup() // ф-я преднастройки
{

  Serial.begin(9600); // установка скорости обмена между ардуино и компуктером

  servo.attach(servoPin); // подвязка пина в серво

  motorInit(stepPinX, dirPinX); // запись 9 и 8 пинов на вывод
  motorInit(stepPinY, dirPinY); //запись 10 и 11 пинов на вывод
  pinMode(2, INPUT_PULLUP); // установка на 2 и 3 пины на вход (концевики)
  pinMode(3, INPUT_PULLUP);

  setZ(1);
  setXY0();
  delay(1000);
  Serial.println("start");// вывод на COM-порт слово старт
}


unsigned int getInt(char* line, int pos) { // считываем значение числа (G, X, Y, I, J)
  unsigned int t = 0;
  int sign = 1;
  if (line[pos] == '-') {
    sign = -1;
    pos++;

  }
  while (isdigit(line[pos]))
  {
    t *= 10;
    t += line[pos++] - '0';
  }
  return sign * t;
}

void processLine(char* line) // обработка команды
{

  int x, y, i_, j_;
  char* indexX;
  char* indexY;

  int i = 0;
  if (line[0] == 'N') {
    while ((line[i] != 'G') && (line[i] != 'M')) {
      i++;
    }
  }


  int cmd = getInt(line, i + 1);

  if (line[i] == 'G')
  {
    switch (cmd)
    {
      case 0: // в этом случае простое перемещение
        indexX = strchr(line, 'X');
        x = getInt(indexX, 1);
        indexY = strchr(line, 'Y');
        y = getInt(indexY, 1);
        setZ(1);
        moveTo(x, y);
        break;

      case 1: // в этом случае - рисуем
        indexX = strchr(line, 'X');
        x = getInt(indexX, 1);
        indexY = strchr(line, 'Y');
        y = getInt(indexY, 1);
        setZ(0);
        moveTo(x, y);
        break;

      case 2:// круговая интерполяция по часовой стрелке
        indexX = strchr(line, 'X');
        x = getInt(indexX, 1);
        indexY = strchr(line, 'Y');
        y = getInt(indexY, 1);
        indexX = strchr(line, 'I');
        i_ = getInt(indexX, 1);
        indexX = strchr(line, 'J');
        j_ = getInt(indexX, 1);
        drawArc(x, y, i_, j_, true);
        break;

      case 3:
        indexX = strchr(line, 'X');
        x = getInt(indexX, 1);
        indexY = strchr(line, 'Y');
        y = getInt(indexY, 1);
        indexX = strchr(line, 'I');
        i_ = getInt(indexX, 1);
        indexX = strchr(line, 'J');
        j_ = getInt(indexX, 1);
        drawArc(x, y, i_, j_, false);
        break;

      default: // действие при неправильном вводе (вывод доступных команд)
        Serial.println(cmd);

    }

  }
  if (line[i] == 'M')
  {
    switch (cmd)

    {
      case 30:
        setZ(1);
        setXY0();
        break;

      default: // действие при неправильном вводе (вывод доступных команд)
        Serial.println(cmd);
    }
  }

}



void loop() {

  const int MAX_LEN = 64; //переменная величины массива
  char line[MAX_LEN]; // объявлениее массива долинной 64 ячейки
  int len = 0;
  bool currentCommand = true;//флаг выхода из цикла
  bool comment = false;
  while (currentCommand) {

    if (Serial.available() > 0) {

      char c = Serial.read();//читаем символ

      switch (c)//смотрим, что прочитали, перебираем
      {
        case '\r':
          break;
        case '\n':
          if ( len > 0) {
            line[len] = '\0';
            processLine(line);//считываем значенике
            currentCommand = false;//заканчиваем цикл
          }
          delay(500);
          Serial.println("OK");
          break;

        case ' ':
          break;

        case '(': //если скобка
          comment = true; // значит это комментарий
          break;

        case ')': //если закрытая скобка
          comment = false; // значит кнец комментария
          break;

        default: // действие по умолчанию
          if (!comment) { // если нет комментария
            if (len < MAX_LEN - 1) {
              line[len++] = ( c >= 'a' && c <= 'z' ) ? c - 'a' + 'A' : c; // маленькие буквы поднимаем в верхний регистр

            }
          }

      } // switch
    }

  }


}





