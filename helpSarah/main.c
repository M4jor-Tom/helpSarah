#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

//#include <kernel.h>

static bool stop_flag = false;
static uint8_t item_count = 0;
K_SEM_DEFINE(prod_sem, 0, 1)

struct item {
	uint8_t a;
	uint8_t b;
};

static struct item last_item;

static void read_from_uart_blocking(char* buf)
{
	/** Cette fonction attend (bloque) jusqu'à ce
	* qu'une chaîne de caractère finissant par '\n'
	* soit reçue sur un bus UART.
	* Ce code est un pseudo-code.
	* Ne pas chercher d'erreur dans cette fonction
	*/
	do
	{
		wait_for_character();
		read_character(buf++);
	} while (!new_line_received());
}

//Arguments inutiles
static void producteur(void* p1, void* p2, void* p3)
{
	char* msg;
	while (!stop_flag)
	{
		read_from_uart_blocking(msg);
		create_item_from_str(msg, &last_item);


		//manque free(msg);


		//Aucun lien entre la fonction en dessous,
		//et la lecture dans msg
		k_sem_give(&prod_sem);
	}
}

static void producteur_start(void)
{
	/** Cette fonction démarre un thread qui exectute
	* la fonction producteur(void *, void *, void *)
	* Ne pas chercher d'erreur dans cette fonction
	*/
	start_thread(producteur);
}

int main(void)
{
	int err;	//<- Jamais utilisé

	producteur_start();

	while (item_count < 5)
	{
		k_sem_take(&prod_sem, K_SECONDS(1));

		printf(
			"New item received: a = %d, b = %d\n",
			last_item.a,
			last_item.b
		);

		processing_item();
	}

	stop_flag = true;
	return 0;
}