/* include globals */
#include	"unpipc.h"

#define	MAXNITEMS 		1000000
#define	MAXNTHREADS			50

		/* globals shared by threads */
int		nitems;				/* read-only by producer and consumer */
int		buff[MAXNITEMS];
struct {
  pthread_mutex_t	mutex;
  int				nput;	/* next index to store */
  int				nval;	/* next value to store */
} put = { PTHREAD_MUTEX_INITIALIZER };

struct {
  pthread_mutex_t	mutex;
  pthread_cond_t	cond;
  int    			nconsumer;
  int 				nget;
  int				nready;	/* number ready for consumer */
} nready = { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0 };
/* end globals */

void	*produce(void *), *consume(void *);

/* include main */
int
main(int argc, char **argv)
{
	int			i, nproducers, nconsumers, countp[MAXNTHREADS], countc[MAXNTHREADS];
	pthread_t	tid_produce[MAXNTHREADS], tid_consume[MAXNTHREADS];

	if (argc != 4)
		err_quit("usage: prodcons6 <#items> <#producers> <#consumers>");
	nitems = min(atoi(argv[1]), MAXNITEMS);
	nproducers = min(atoi(argv[2]), MAXNTHREADS);
	nconsumers = min(atoi(argv[3]), MAXNTHREADS);

	Set_concurrency(nproducers + nconsumers);
		/* 4create all producers and one consumer */
	for (i = 0; i < nproducers; i++) {
		countp[i] = 0;
		Pthread_create(&tid_produce[i], NULL, produce, &countp[i]);
	}
	for (i = 0; i < nconsumers; i++) {
		countc[i] = 0;
		Pthread_create(&tid_consume[i], NULL, consume, &countc[i]);
	}

		/* wait for all producers and the consumer */
	for (i = 0; i < nproducers; i++) {
		Pthread_join(tid_produce[i], NULL);
		printf("countp[%d] = %d\n", i, countp[i]);	
	}
	for (i = 0; i < nconsumers; i++) {
		Pthread_join(tid_consume[i], NULL);
		printf("countc[%d] = %d\n", i, countc[i]);
	}

	exit(0);
}
/* end main */

/* include prodcons */
void *
produce(void *arg)
{
	for ( ; ; ) {
		Pthread_mutex_lock(&put.mutex);
		if (put.nput >= nitems) {
			Pthread_mutex_unlock(&put.mutex);
			return(NULL);		/* array is full, we're done */
		}
		buff[put.nput] = put.nval;
		put.nput++;
		put.nval++;
		Pthread_mutex_unlock(&put.mutex);

		Pthread_mutex_lock(&nready.mutex);
		if (nready.nready == 0)
			Pthread_cond_signal(&nready.cond);
		nready.nready++;
		Pthread_mutex_unlock(&nready.mutex);

		*((int *) arg) += 1;
	}
}

void *
consume(void *arg)
{
	for ( ; ; ) {
		Pthread_mutex_lock(&nready.mutex);
		if (nready.nconsumer > 0) {
			Pthread_mutex_unlock(&nready.mutex);
			continue;
		} else if (nready.nget >= nitems) {
			Pthread_mutex_unlock(&nready.mutex);
			return(NULL);	
		} else {
			nready.nconsumer++;
			while (nready.nready == 0)
				Pthread_cond_wait(&nready.cond, &nready.mutex);
				nready.nready--;
				nready.nget++;
				nready.nconsumer--;
				Pthread_mutex_unlock(&nready.mutex);

				// if (buff[i] != i)
				// 	printf("buff[%d] = %d\n", i, buff[i]);
			
				*((int *) arg) += 1;
			}
	}
	
	return(NULL);
}
/* end prodcons */
