%symulacje badanie polaryzacji na roznych wielkosciach stada
close all;


x= [100	484	1024	2025	3025	3969	5041	7569	10000	19881	40000	50176];
y1 = yval("0,5/");
y2 = yval("0,75/");
y3 = yval("1/");
y4 = yval("1,25/");
y5 = yval("1,5/");
y6 = yval("2/");
y7 = yval("2,5/");
y8 = yval("3/");
y9 = yval("4/");
y10= yval("5/");
y11= yval("7,5/");
y12= yval("10/");

figure;
hold on;
plot(x,y1 ,"*-r");
plot(x,y2 ,"*-g");
plot(x,y3 ,"*-b");
plot(x,y4 ,"*-k");
plot(x,y5 ,"*-y");
plot(x,y6 ,"*-m");

hold off;

legend ("0,5", "0,75", "1", "1,25", "1,5", "2");
xlim([100 50176]);
ylim([0 0.05]);
xlabel("wielkość tłumu");
ylabel("czas obliczenia kroku");

figure;
hold on;
plot(x,y7 ,"*-r");
plot(x,y8 ,"*-g");
plot(x,y9 ,"*-b");
plot(x,y10,"*-k");
plot(x,y11,"*-y");
plot(x,y12,"*-m");
hold off;
legend ("2,5", "3", "4", "5", "7,5", "10");
xlim([100 50176]);
ylim([0 0.05]);
xlabel("wielkość tłumu");
ylabel("czas obliczenia kroku");


