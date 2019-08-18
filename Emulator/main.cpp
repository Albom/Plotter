#include <cstdio>
#include <cstdlib>
#include <cmath>

#define X 0
#define Y 1

int currX = 0;
int currY = 0;
int Z = 1;

FILE *fOut;

void motorStep(int stepPin, bool forward)
{

    fprintf(fOut, "%5d %5d ", currX, currY);

    int deltaX = 0;
    int deltaY = 0;

    /// !!!
    if (stepPin == X)
    {
        deltaX = forward ? 1 : -1;
        currX += deltaX;
    }
    else
    {
        deltaY = forward ? 1 : -1;
        currY += deltaY;
    }


    fprintf(fOut, "%5d %5d %2d\n", deltaX, deltaY, Z);
}


void moveTo(int x, int y)
{

    const int deltaX = abs(x - currX);
    const int deltaY = abs(y - currY);
    const int signX = currX < x ? 1 : -1;
    const int signY = currY < y ? 1 : -1;
    int error = deltaX - deltaY;

    while (currX != x ||  currY != y)
    {
        int error2 = error * 2;
        if (error2 > -deltaY)
        {
            error -= deltaY;
            /// !!!
            // currX += signX;

            motorStep(X, signX == 1);
            //delay(10);
        }

        if ( error2 < deltaX)
        {
            error += deltaX;
            /// !!!
            // currY += signY;

            motorStep(Y, signY == 1);
            //delay(10);
        }

    }

}

void setZ(int z)
{
    Z = z;
}

void drawLine(int x1, int y1, int x2, int y2)
{
    if ((currX == x1) && (currY == y1))
    {
        setZ(0);
        moveTo(x2, y2);
    }
    else
    {
        setZ(1);
        moveTo(x1, y1);
        setZ(0);
        moveTo(x2, y2);
    }
}



void drawCircle(int x0, int y0, int r)
{
    Z = 1;
    moveTo(x0+r, y0);
    Z = 0;

    for (float theta = 0; theta <= 2*M_PI; theta += M_PI/100.0)
    {
        int x = x0 + r*cosf(theta);
        int y = y0 + r*sinf(theta);
        printf("%d\n",x);
        printf("%d\n",y);
        printf("%d\n",r);
        moveTo(x, y);
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
    float radius=sqrt(I*I+J*J);

    printf("%f %f %f\n", cx, cy, radius);

    // найти развертку дуги
    float angle1 = atan2f(J,I)-M_PI;//абсолютное значение угла
    float angle2 = atan2f(y-cy,x-cx);
    float sweep = angle2-angle1;

    if(dir && sweep<0)
        angle2+=2*M_PI;
    else if(!dir && sweep>0)
        angle1+=2*M_PI;

    sweep=angle2-angle1;

    // get length of arc
    // float circumference=PI*2.0*radius;
    // float len=sweep*circumference/(PI*2.0);
    // simplifies to
    float len = abs(sweep) * radius;

    int num_segments = floor(len);

    for(int i=0; i<num_segments; ++i)
    {
        // interpolate around the arc
        float fraction = ((float)i)/((float)num_segments);
        float angle3 = ( sweep * fraction ) + angle1;

        // find the intermediate position
        float nx = cx + cos(angle3) * radius;
        float ny = cy + sin(angle3) * radius;
        // make a line to that intermediate position
        moveTo(nx,ny);
    }

    // one last line hit the end
    //line(x,y,posz);
}


//void drawArc(float cx,float cy,float x,float y, bool dir)
//{
//
//    setZ(0);
//
//    // cx, cy - центр
//    // x, y - конечные координаты
//    // dx, dy - I, J
//    float dx = currX - cx;
//    float dy = currY - cy;
//    float radius=sqrt(dx*dx+dy*dy);
//
//    // найти развертку дуги
//    float angle1 = atan2f(dy,dx);//абсолютное значение угла
//    float angle2 = atan2f(y-cy,x-cx);
//    float sweep = angle2-angle1;
//
//    if(dir && sweep<0)
//        angle2+=2*M_PI;
//    else if(!dir && sweep>0)
//        angle1+=2*M_PI;
//
//    sweep=angle2-angle1;
//
//    // get length of arc
//    // float circumference=PI*2.0*radius;
//    // float len=sweep*circumference/(PI*2.0);
//    // simplifies to
//    float len = abs(sweep) * radius;
//
//    int num_segments = floor(len);
//
//    for(int i=0; i<num_segments; ++i)
//    {
//        // interpolate around the arc
//        float fraction = ((float)i)/((float)num_segments);
//        float angle3 = ( sweep * fraction ) + angle1;
//
//        // find the intermediate position
//        float nx = cx + cos(angle3) * radius;
//        float ny = cy + sin(angle3) * radius;
//        // make a line to that intermediate position
//        moveTo(nx,ny);
//    }
//
//    // one last line hit the end
//    //line(x,y,posz);
//}



/*
 setZ(0);
//float p = sqrtf((float)(currX-x)*(currX-x)+(currY-y)*(currY-y));
//float r = sqrtf((float)(i)*(float)(i) +(float) (j)*(float)(j));
//float alpha = acosf((r*r+r*r-p*p)/(2*r*r));
float theta1=atanf(float(currY)/float(currX));
float theta2= atanf(float(y)/float(x));
int cX,cY;

cX=15;
cY=25;
printf("%d\n",cX);
printf("%d\n",cY);
//moveTo(cX,cY);
//printf("%g\n",p);
//printf("%g\n",r);
//printf("%g\n",alpha);
//printf("%g\n",theta1);
int x_ = 0;
int y_= 0;
 //printf("%d\n",x_);
   // printf("%d\n",y_);
   // moveTo(x_,y_);

 for (float i = theta2;  i<=theta1; i += 2*M_PI / 100.0)
 {
     //printf("%g\n",theta);

    x_=cX+ r* cosf(i);
    y_= cY+r* sinf(i);

    printf("%d\n",x_);
    printf("%d\n",y_);

    moveTo(x_, y_);



}

 }
*/

int main()
{

    fOut = fopen("out.txt", "wt");

    setZ(1);
    moveTo(0,30);
    setZ(0);
    drawLine(0,30,10,0);
    drawLine(10,0,20,30);
    setZ(1);
    moveTo(17,22);
    setZ(0);
    drawLine(17,22,3,22);
    setZ(1);
    moveTo(30,30);
    setZ(0);
    drawLine(30,30,30,0);
    drawLine(30,0,40,10);
    drawLine(40,10,50,0);
    drawLine(50,0,50,30);


//    drawLine(0,0,10,30);
//    drawLine(10,30,20,0);
//    moveTo(17,8);
//    drawLine(17,8,3,8);
    //moveTo(15, 15);

//drawCircle(60, 40, 35);
//    drawArc(35,15,10,15,true);
//    drawArc(35,45,-10,15,true);
//    drawArc(15,45,-10,-15,!true);
//   drawArc(15,15,10,-15,true);
//    drawArc(15,45,10,15,!true);
//    drawArc(35,45,10,-15,!true);
//    drawArc(35,15,-10,-15,!true);
//    drawArc(15,15,-10,15,!true);

  // drawArc(15,19,10, 10, !true);

    fclose(fOut);

    return 0;
}
