close all;

%%
x= [100	484	1024	2025	3025	3969	5041	10000	19881	40000	60025];
y1 = yval("cache/0,33/");
y2 = yval("cache/0,5/");
y3 = yval("cache/1/");
y4 = yval("cache/2/");
y5 = yval("cache/5/");
y6 = yval("cache/10/");

y1b = yval("0,33/");
y2b = yval("0,5/");
y3b = yval("1/");
y4b = yval("2/");
y5b = yval("5/");
y6b = yval("10/");

b = yval("simple/");
%%
figure;
hold on;
plot(x,y1 ,"*-r");
plot(x,y2 ,"*-g");
plot(x,y3 ,"*-b");
plot(x,y4 ,"*-m");
plot(x,y5 ,"*-k");
plot(x,y6 ,"*-y");
plot(x,b ,".-r");
hold off;

legend ("0,33","0,5","1","2","5","10");
xlim([100 60025]);

xlabel("wielkość tłumu");
ylabel("czas obliczenia kroku");
%%
figure;
hold on;
plot(x,y1b ,"*-r");
plot(x,y2b ,"*-g");
plot(x,y3b ,"*-b");
plot(x,y4b ,"*-m");
plot(x,y5b ,"*-k");
plot(x,y6b ,"*-y");

hold off;

legend ("0,33","0,5","1","2","5","10");
xlim([100 60025]);

xlabel("wielkość tłumu");
ylabel("czas obliczenia kroku");


figure;
plot(x,b ,"*-r");

legend ("simple");
xlim([100 10000]);
ylim([0 0.6]);
xlabel("wielkość tłumu");
ylabel("czas obliczenia kroku");


z1 = mean(y1);
z2 = mean(y2);
z3 = mean(y3);
z4 = mean(y4);
z5 = mean(y5);
z6 = mean(y6);

zb1 = mean(y1b);
zb2 = mean(y2b);
zb3 = mean(y3b);
zb4 = mean(y4b);
zb5 = mean(y5b);
zb6 = mean(y6b);

bm = mean(b);

xz = [0.33 0.5 1 2 5 10];
z = [z1 z2 z3 z4 z5 z6];
zb = [zb1 zb2 zb3 zb4 zb5 zb6];
zc = [bm bm bm bm bm bm];
figure;
hold on;
plot(xz,z,"*-r");
plot(xz,zb,"*-b");
hold off;

figure;
[xx, yy] = meshgrid (xz, x);
xd= [y1;
     y2;
     y3;
     y4;
     y5;
     y6];

mesh (x, xz, xd);
xlabel ("crowd size");
ylabel ("cell size");
zlabel ("compute frame time");
