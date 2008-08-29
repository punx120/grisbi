#ifndef _TRANSACTION_LIST_H
#define _TRANSACTION_LIST_H (1)


/* START_INCLUDE_H */
/* END_INCLUDE_H */

/* START_DECLARATION */
void transaction_list_append_archive (gint archive_store_number);
void transaction_list_append_transaction ( gint transaction_number );
void transaction_list_colorize (void);
void transaction_list_filter ( gint account_number );
gboolean transaction_list_remove_archive ( gint archive_number );
gboolean transaction_list_remove_transaction ( gint transaction_number );
void transaction_list_set ( GtkTreeIter *iter,
			    ... );
void transaction_list_set_balances ( void );
gboolean transaction_list_show_toggle_mark ( gboolean show );
gboolean transaction_list_update_cell ( gint cell_col,
					gint cell_line );
gboolean transaction_list_update_column ( gint column,
					  GValue *value );
gboolean transaction_list_update_element ( gint element_number );
gboolean transaction_list_update_transaction ( gint transaction_number );
/* END_DECLARATION */
#endif
