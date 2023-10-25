#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

const int Ntel = 3; // max telephonites
const int Ncook = 2; // maximum cooks
const int Noven = 10; // maximum ovens
const int Ndeliverer = 7; //maximum deliverers
const int Npack=1 ; // pack guy

const int T_orderlow = 1; // [T_order_low - T_order_high] : time range in which a new order arrives
const int T_orderhigh = 5;

const int N_orderlow = 1; // [N_order_low - N_order_high] : range of possible number of pizzas 
const int N_orderhigh = 5;

const int T_paymentlow = 1; // time range in which the card is charged with the order
const int T_paymenthigh = 2;

const float Pfail = 0.05; // chance card be declined 

const int C_pizza=10; // the cost of one pizza

const int T_prep = 1; // preparation time for each pizza

const int T_bake = 10; // baking time for each pizza

const int T_pack= 2;//packing time for each pizza

const int T_dellow = 5; // [T_dellow - T_delhigh] : delivery time 
const int T_delhigh = 15;






