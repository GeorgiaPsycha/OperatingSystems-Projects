#include "p3190225-pizza.h"
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>



//mutexes init 
	pthread_mutex_t mutex_tel = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_cooks = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_ovens = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_deliverers = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_pack=PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_t mutex_screen=PTHREAD_MUTEX_INITIALIZER;
	
	
// conditions init
	pthread_cond_t cond_tel = PTHREAD_COND_INITIALIZER;
	pthread_cond_t cond_cooks = PTHREAD_COND_INITIALIZER;
	pthread_cond_t cond_ovens = PTHREAD_COND_INITIALIZER;
	pthread_cond_t cond_deliverers = PTHREAD_COND_INITIALIZER;
	pthread_cond_t cond_pack = PTHREAD_COND_INITIALIZER;
	
//initialize to sleep
unsigned int sleep(unsigned int seconds);

// number of customers
int N_cust;

//Declaration of variables

int total_money ; // ta esoda ths pizzarias

int N_tel = Ntel; //  telephonites
int N_cook = Ncook; //  cooks
int N_oven = Noven; // ovens
int N_deliver = Ndeliverer; //deliverers
int N_pack= Npack; // pack guy
int success_order; // plh8os success order
int fail_order;// plh8os fail order 
// variables for calculating max and average wait time
int max_wait_time=0;
int total_wait_time=0;
// variables for calculating max and average service time
int customer_service_time=0;
int total_customer_service_time =0;
int max_customer_service_time =0 ;
// variables for calculating max and average time where pizza cold from oven to finally delivered
int  cold_time=0;
int total_cold_time=0;
int max_cold_time=0;

double avg_wait =0 ;
double avg_cold=0;
double avg_service=0;

int prep_min=0;
unsigned int seed;



void *order(void *x){

    int oid = *(int *)x;
    int rc;
    
    seed= seed+oid; // every order has a unique seed
    
    printf("Hello order: %d\n",oid);
    
    // structs for time
    struct timespec order_start; // the customer just called

    struct timespec time_prep; // when the order started being prepared
    struct timespec prep_stop; // when the order stoped being prepared
    struct timespec customer_wait; // when the customer finally connects with a phone
    struct timespec order_stop; // when the order is delivered to the adress
    struct timespec time_baked; // when the pizza is baked
    
   
    //get the time the customer just showed up
    clock_gettime(CLOCK_REALTIME, &order_start);
    
    // lock telephone mutex 
    rc = pthread_mutex_lock(&mutex_tel);
    
    // check for available tel
    while(N_tel==0){
           //printf(" No available telephones ... please wait \n");
           rc = pthread_cond_wait(&cond_tel, &mutex_tel);   
    }
    //printf("Telephone was found for order %d.\n", oid);
    N_tel--; // occupies the Telephone
    
    //get the time the customer just find the telephone to speak
    clock_gettime(CLOCK_REALTIME, &customer_wait);
    
    
    // find the max wait time and add the to total for the average later
    int wait_time= (customer_wait.tv_sec-order_start.tv_sec);
    
    total_wait_time = total_wait_time + wait_time;
    if (wait_time> max_wait_time){
       max_wait_time= wait_time;
          
    }
   
    // how many pizzas need to be prepared find with the random seed
    int pizzas = rand_r(&seed) % (N_orderhigh +1 - N_orderlow)+ N_orderlow;
    
    //wait time for payment
    int pay_time= rand_r(&seed) % (T_paymenthigh + 1-T_paymentlow) + T_paymentlow;
    rc = pthread_mutex_unlock(&mutex_tel);
    sleep(pay_time); // wait now
    
    
   
 
    
    //payment process 
  	
  	//calculate the 5% odd so card declined
	if(rand_r(&seed) %(100 +1)<5)
	{
	// card declined
	  rc = pthread_mutex_lock(&mutex_tel);
	  rc = pthread_mutex_lock(&mutex_screen);// lock screen mutex
	  printf ("The order with id  %d failed \n",oid); // print the wanted message
	  fail_order++; // add 1 to total fail orders 
	  N_tel++;// free the telephoner
	  rc = pthread_cond_signal(&cond_tel); // free the telephoner for next customer
      rc = pthread_mutex_unlock(&mutex_tel);
      rc = pthread_mutex_unlock(&mutex_screen);
      pthread_exit(NULL);  // end the thread
	  
	}
	 
	 // payment was successfull
	 
     rc = pthread_mutex_lock(&mutex_tel);	
     rc = pthread_mutex_lock(&mutex_screen);  
     printf ("Customer %d your order is starting prepared now \n",oid); // print the wanted message
	 rc = pthread_mutex_unlock(&mutex_screen);
	 success_order++; // add 1 to total succes orders 
    
     
     N_tel++;// free the telephoner
    
    // raise the earn money    
    total_money= total_money + C_pizza*pizzas;
    // free the telephoner for next customer
    rc = pthread_cond_signal(&cond_tel);
    rc = pthread_mutex_unlock(&mutex_tel);// unlock telephone mutex
    
    
    // time to find our cooks
    
    rc = pthread_mutex_lock(&mutex_cooks);
    // check for available cooks 
    while(N_cook==0){
           //printf(" No available cooks ... please wait \n");
           // wait for the available cooks
           rc = pthread_cond_wait(&cond_cooks, &mutex_cooks);   
    }
    
    N_cook--; // occupies the cook
    rc = pthread_mutex_unlock(&mutex_cooks); // unlock mutex so we can work in sleep
    
    //get the time the order was started to be prepared
    clock_gettime(CLOCK_REALTIME, &time_prep);
    
    //wait time for preparation of pizzas
    sleep(T_prep*pizzas);
    rc = pthread_mutex_lock(&mutex_cooks);
    
    
    // its time for bake 
    // lets find a oven  
    rc = pthread_mutex_lock(&mutex_ovens);
    //check for enough available ovens for every pizza
    while(N_oven<pizzas){
           //printf(" No available ovens ... please wait \n");
           rc = pthread_cond_wait(&cond_ovens, &mutex_ovens);   
    }
   
     N_cook++; // free the cooks the pizzas are in oven
     rc = pthread_mutex_unlock(&mutex_cooks);
     rc = pthread_cond_signal(&cond_cooks); // free ton poro cooks for the next thread
   
     // start the bake
     
     N_oven= N_oven-pizzas;// ocupied the ovens
     
     
     rc = pthread_mutex_unlock(&mutex_ovens);
   
     // all the  pizzas bake at the same time
    sleep( T_bake);
      
     //get the time the order is out of ovens
    clock_gettime(CLOCK_REALTIME, &time_baked);
   
    rc = pthread_mutex_lock(&mutex_ovens);
    N_oven=N_oven+pizzas;//free the ovens
    rc = pthread_mutex_unlock(&mutex_ovens);
    // boardcast beacause i wonna free multiple ovens
    rc = pthread_cond_broadcast(&cond_ovens);
    
   
    //wait time for preparation of pizzas
    sleep(T_prep*pizzas);
    
    //pack time
    rc = pthread_mutex_lock(&mutex_pack);
    // check if the pack quy is available and if he is not, wait
     while(N_pack==0){
           //printf(" No available pack quy ... please wait \n\n");
           rc = pthread_cond_wait(&cond_pack, &mutex_pack);   
    }
    
    N_pack--;// ocupies the one and only pack quy
    rc = pthread_mutex_unlock(&mutex_pack);
    sleep(T_pack*pizzas);
    
    //get the time the order is ready to be delivered (the preparation stopped)
    clock_gettime(CLOCK_REALTIME, &prep_stop);
    
    rc = pthread_mutex_lock(&mutex_pack);
    // calculate the time the pizza is being prepared
    prep_min = prep_stop.tv_sec- order_start.tv_sec;
    
    rc = pthread_mutex_lock(&mutex_screen);
    printf(" Order with id %d packed sussefully in %d minutes \n\n",oid,prep_min);  
    rc = pthread_mutex_unlock(&mutex_screen);
    
    N_pack++; // free the pack quy
    rc = pthread_mutex_unlock(&mutex_pack);
    rc = pthread_cond_signal(&cond_pack); // free the pack quy for the next thread
    
    // deliver time 
    
    rc = pthread_mutex_lock(&mutex_deliverers);
    // check if there is any delivery quy is available and if it's not ,wait
     while(N_deliver==0){
           //printf(" No available delivery quy ... please wait \n");
           rc = pthread_cond_wait(&cond_deliverers, &mutex_deliverers);   
    }
    
    N_deliver--; // occupies the delivery
    rc = pthread_mutex_unlock(&mutex_deliverers);
    // calculate the random time for delivery
    int del_time= rand_r(&seed) % (T_delhigh + 1 - T_dellow)+ T_dellow;
    
    // the time the delivery needs to go to the adrress
    sleep(del_time);
    
    //get the time the order just delivered 
    clock_gettime(CLOCK_REALTIME, &order_stop);
    
    // find max avg cystomer service time
    int d_time= (order_stop.tv_sec - order_start.tv_sec);
  
    rc = pthread_mutex_lock(&mutex_screen);
    printf("Order %d delivred sussefully in %d minutes \n\n",oid,d_time); 
    rc = pthread_mutex_unlock(&mutex_screen);
    
    // finds the max and total time for pizza to delivery to the adress 
    total_customer_service_time= total_customer_service_time + d_time;
    if (d_time> max_customer_service_time)
    { max_customer_service_time=d_time;
    }
    // find max ,avg cold time (waiting out of oven until baked
    cold_time= order_stop.tv_sec - time_baked.tv_sec;
    
    total_cold_time=  total_cold_time+ cold_time;
    if ( max_cold_time < cold_time){
    	 max_cold_time = cold_time;
    }  
    
    //the time the delivery guy  needs to return from the adrress
    sleep(del_time);
    rc = pthread_mutex_lock(&mutex_deliverers);
   
     
    // free ton poro + unlock
    N_deliver++;// delivery quy available again
    rc = pthread_mutex_unlock(&mutex_deliverers);
    rc = pthread_cond_signal(&cond_deliverers);
    
    // exit 
    pthread_exit(NULL);

  
}

int main(int argc, char *argv[]) {

	// check the number of arguments
	if (argc != 3) {
		printf("Please enter only two arguments.\n\n");
		exit(-1);
	}

	// save the parametres
	N_cust = atoi(argv[1]);
	seed = atoi(argv[2]);

	// check for the right arguments
	if(N_cust <0) {
        printf("Customer Negative number.\n\n");
        exit(-1);
        }
        
	// create the id for every customer
	int customer_id[N_cust];
	for (int i = 0; i < N_cust; i++) {
		customer_id[i] = i + 1;
        }
	
	int rc;
	pthread_t threads[N_cust];
	
	//initialize order threads

         pthread_t *thread;
         thread = malloc(N_cust*sizeof(pthread_t));
         if(thread==NULL){
            printf("Error in memory.\n\n");
            exit(-1);
         }

	for (int i = 0; i < N_cust; i++)
        {
		
		if (i==0){
		//printf("Main:Δημιουργία νηματος για πελατη  %d\n\n", i+1);
		rc = pthread_create(&threads[i], NULL, &order, &customer_id[i]);
		
		}else{
		//printf("Main:Δημιουργία νηματος για πελατη  %d\n\n", i+1);
		int next_order = rand_r(&seed)% (T_orderhigh+1-T_orderlow )+ T_orderlow;
		sleep(next_order);
		rc = pthread_create(&threads[i], NULL, &order, &customer_id[i]);

		}
	}
	
	
	// joining threads so main wait for threads to finish 
	void *stat;
	for (int i = 0; i < N_cust; i++) {
		pthread_join(threads[i], &stat);
	}
	
    
    // messages
    rc = pthread_mutex_lock(&mutex_screen);
	printf("\nThe pizzaria had made %d money today and had %d successfull orders and %d fail orders.\n\n", total_money, success_order,fail_order);
	avg_wait = total_wait_time/N_cust;
	printf("\nThe max wait time was %d and the average was %f.\n\n",max_wait_time,avg_wait);
	avg_service=total_customer_service_time/success_order;
	printf("\n The max service time was %d and the average was %f.\n\n",max_customer_service_time,avg_service);
	avg_cold=total_cold_time/success_order;
	printf("\nThe max cold time was %d and the average was %f.\n\n",max_cold_time,avg_cold);
	rc = pthread_mutex_unlock(&mutex_screen);
	
	free(thread);// FREE memory
	// destroy cond, mutex
	pthread_mutex_destroy(&mutex_tel);
    pthread_cond_destroy(&cond_tel);
    pthread_mutex_destroy(&mutex_cooks);
    pthread_cond_destroy(&cond_cooks);
    pthread_mutex_destroy(&mutex_ovens);
    pthread_cond_destroy(&cond_ovens);
    pthread_mutex_destroy(&mutex_deliverers);
    pthread_cond_destroy(&cond_deliverers);
    pthread_mutex_destroy(&mutex_pack);
    pthread_cond_destroy(&cond_pack);
    pthread_mutex_destroy(&mutex_screen);
   

    return 0;
}
