/* ************************************************************************** */
/*                                                                            */
/*     copyright (c)	2000-2006 Cédric Auger (cedric@grisbi.org)	      */
/*			2004-2006 Benjamin Drieu (bdrieu@april.org) 	      */
/*			http://www.grisbi.org   			      */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or modify      */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation; either version 2 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*                                                                            */
/* ************************************************************************** */

/**
 * \file gsb_form_transaction.c
 * this file contains function of form for transactions (not scheduled transactions)
 */


#include "include.h"

/*START_INCLUDE*/
#include "gsb_form_transaction.h"
#include "./gsb_currency.h"
#include "./gsb_data_account.h"
#include "./gsb_data_currency_link.h"
#include "./gsb_data_form.h"
#include "./gsb_data_payee.h"
#include "./gsb_data_report.h"
#include "./gsb_data_transaction.h"
#include "./gsb_form.h"
#include "./gsb_form_widget.h"
#include "./gsb_payment_method.h"
#include "./gsb_real.h"
#include "./gsb_transactions_list.h"
#include "./gtk_combofix.h"
#include "./dialog.h"
#include "./etats_calculs.h"
#include "./gtk_combofix.h"
#include "./gsb_data_form.h"
#include "./include.h"
#include "./erreur.h"
#include "./structures.h"
#include "./gsb_real.h"
#include "./gsb_form_widget.h"
/*END_INCLUDE*/

/*START_STATIC*/
/*END_STATIC*/

/*START_EXTERN*/
/*END_EXTERN*/


/**
 * if only the date and payee are filled in the form, fill all the fields with
 * the same datas that the last transaction found with the same payee
 *
 * \param payee_name
 *
 * \return FALSE if no payee name or problem (and the entry payee will be erased), TRUE if ok
 * */
gboolean gsb_form_transaction_complete_form_by_payee ( const gchar *payee_name )
{
    gint payee_number;
    gint transaction_number;
    gint account_number;
    GSList *tmp_list;

    gchar* tmpstr = g_strdup_printf ( "gsb_form_transaction_complete_form_by_payee %s",
				    payee_name );
    devel_debug ( tmpstr );
    g_free(tmpstr);

    if ( !strlen (payee_name))
	return FALSE;

    account_number = gsb_form_get_account_number ();

    /* check if another field than date and payee is filled */
    tmp_list = gsb_form_widget_get_list ();
    while (tmp_list)
    {
	struct_element *element;

	element = tmp_list -> data;

	if ( element -> element_number
	     &&
	     element -> element_number != TRANSACTION_FORM_DATE
	     &&
	     element -> element_number != TRANSACTION_FORM_PARTY
	     &&
	     ((GTK_IS_ENTRY (element -> element_widget)
	       &&
	       !gsb_form_widget_check_empty(element -> element_widget))
	      ||
	      (GTK_IS_COMBOFIX (element -> element_widget)
	       &&
	       !gsb_form_widget_check_empty(GTK_COMBOFIX (element -> element_widget) -> entry))))
	    return TRUE;

	tmp_list = tmp_list -> next;
    }

    /* get the payee_number */
    payee_number = gsb_data_payee_get_number_by_name ( payee_name,
						       FALSE );
    /* if it's a new payee, go away */
    if ( !payee_number )
	return TRUE;

    /* find the last transaction with that payee */
    transaction_number = gsb_form_transactions_look_for_last_party ( payee_number,
								     0,
								     account_number );

    /* if no same transaction, go away */
    if ( !transaction_number )
	return TRUE;

    /* fill the form */
    tmp_list = gsb_form_widget_get_list ();
    while (tmp_list)
    {
	struct_element *element;

	element = tmp_list -> data;

	if ( element -> element_number != TRANSACTION_FORM_OP_NB
	     &&
	     element -> element_number != TRANSACTION_FORM_DATE
	     &&
	     element -> element_number != TRANSACTION_FORM_VALUE_DATE
	     &&
	     element -> element_number != TRANSACTION_FORM_EXERCICE
	     &&
	     element -> element_number != TRANSACTION_FORM_PARTY
	     &&
	     element -> element_number != TRANSACTION_FORM_MODE )
	    gsb_form_fill_element ( element -> element_number,
				    account_number,
				    transaction_number,
				    TRUE );

	tmp_list = tmp_list -> next;
    }
    return TRUE;
}



/**
 * Look for the last transaction with the same party. Begin in the current account,
 * and continue in other accounts if necessary.
 *
 * \param no_party the party we are looking for
 * \param no_new_transaction if the transaction found is that transaction, we don't
 * \param account_number the account we want to find first the party
 * keep it
 *
 * \return the number of the transaction found, or 0 
 * */
gint gsb_form_transactions_look_for_last_party ( gint no_party,
						 gint no_new_transaction,
						 gint account_number )
{
    GSList *list_tmp_transactions;
    gint last_transaction_with_party_in_account = 0;
    gint last_transaction_with_party_not_in_account = 0;

    list_tmp_transactions = gsb_data_transaction_get_complete_transactions_list ();

    while ( list_tmp_transactions )
    {
	gint transaction_number_tmp;
	transaction_number_tmp = gsb_data_transaction_get_transaction_number (list_tmp_transactions -> data);

	if ( gsb_data_transaction_get_party_number (transaction_number_tmp) == no_party
	     &&
	     transaction_number_tmp != no_new_transaction
	     &&
	     !gsb_data_transaction_get_mother_transaction_number (transaction_number_tmp)
	     &&
	     gsb_data_transaction_get_breakdown_of_transaction (transaction_number_tmp))
	{
	    /* we are on a transaction with the same party, it's also a breakdown, so we keep it */
	    if ( gsb_data_transaction_get_account_number (transaction_number_tmp) == account_number)
		last_transaction_with_party_in_account = transaction_number_tmp;
	    else
		last_transaction_with_party_not_in_account = transaction_number_tmp;
	}
	list_tmp_transactions = list_tmp_transactions -> next;
    }

    if ( last_transaction_with_party_in_account )
	return last_transaction_with_party_in_account;

    /* if we don't want to complete with a transaction in another account,
     * go away here */
    if ( etat.limit_completion_to_current_account )
	return 0;

    return last_transaction_with_party_not_in_account;
}


/**
 * check if the currency given in param is the same of the account currency
 * or if there is a link between them
 * if not, show the change button
 *
 * \param currency_number the currency of the transaction
 * \param account_number 
 *
 * \return
 * */
void gsb_form_transaction_check_change_button ( gint currency_number,
						gint account_number )
{
    gint account_currency_number;

    account_currency_number = gsb_data_account_get_currency (account_number);

    if ( gsb_data_currency_link_search ( account_currency_number,
					 currency_number ))
	gtk_widget_hide ( gsb_form_widget_get_widget (TRANSACTION_FORM_CHANGE));
    else
	gtk_widget_show ( gsb_form_widget_get_widget (TRANSACTION_FORM_CHANGE));
}



/**
 * Get a breakdown of transactions and ask if we want to clone the daughters
 * do the copy if needed, set for the daugthers the number of the new transaction
 * 
 * \param new_transaction_number the number of the new mother of the cloned transaction
 * \param no_last_breakdown the no of last breakdown mother
 * \param no_account the account of the last breakdown mother (important if it's not the same of the new_transaction)
 *
 * \return FALSE
 * */
gboolean gsb_form_transaction_recover_breakdowns_of_transaction ( gint new_transaction_number,
								  gint no_last_breakdown,
								  gint no_account )
{
    gint result;
    GSList *list_tmp_transactions;

    /* FIXME : check for win, normally should work fine now because
     * that dialog is showed after all, and not when losing focus */
    result = question_conditional_yes_no ( "recover-breakdown" );

    if ( !result )
	return FALSE;

    /* go around the transactions list to get the daughters of the last breakdown */
    list_tmp_transactions = gsb_data_transaction_get_complete_transactions_list ();

    while ( list_tmp_transactions )
    {
	gint transaction_number_tmp;
	transaction_number_tmp = gsb_data_transaction_get_transaction_number (list_tmp_transactions -> data);

	if ( gsb_data_transaction_get_account_number (transaction_number_tmp) == no_account
	     &&
	     gsb_data_transaction_get_mother_transaction_number (transaction_number_tmp) == no_last_breakdown)
	{
	    gsb_data_transaction_set_mother_transaction_number ( gsb_transactions_list_clone_transaction (transaction_number_tmp),
								 new_transaction_number);
	}
	list_tmp_transactions = list_tmp_transactions -> next;
    }
    return FALSE;
}


/**
 * return a list of numbers of parties if the party in the form is a 
 * report
 * 
 * \param none
 * 
 * \return a g_slist, with -1 if it's a normal party or a list of parties if it's a report
 * */
GSList *gsb_form_transaction_get_parties_list_from_report ( void )
{
    GSList *parties_list;

    parties_list = NULL;

    /*     check that the party's form exist, else, append -1 and go away */
    if ( gsb_data_form_check_for_value (TRANSACTION_FORM_PARTY))
    {
	const gchar *string;

	string = gtk_combofix_get_text ( GTK_COMBOFIX ( gsb_form_widget_get_widget (TRANSACTION_FORM_PARTY)));

	if ( strncmp ( string,
		       _("Report : "),
		       7 ))
	    /* the party is not a report, set -1 and go away */
	    parties_list = g_slist_append (parties_list,
					   GINT_TO_POINTER (-1));
	else
	{
	    /* it's a report */

	    gchar **tab_char;
	    gint report_number;

	    tab_char = g_strsplit ( string,
				    " : ",
				    2 );

	    report_number = gsb_data_report_get_report_by_name (tab_char[1]);

	    if (report_number)
	    {
		GSList *list_transactions;
		GSList *list_tmp;

		list_transactions = recupere_opes_etat (report_number);

		list_tmp = list_transactions;

		while ( list_tmp )
		{
		    gint transaction_number;

		    transaction_number = gsb_data_transaction_get_transaction_number (list_tmp -> data);

		    if ( !g_slist_find ( parties_list,
					 GINT_TO_POINTER (gsb_data_transaction_get_party_number (transaction_number))))
			parties_list = g_slist_append ( parties_list,
							GINT_TO_POINTER ( gsb_data_transaction_get_party_number (transaction_number)));
		    list_tmp = list_tmp -> next;
		}
		g_slist_free ( list_transactions );
	    }
	    else
		/* the report was not found, set -1 */
		parties_list = g_slist_append (parties_list,
					       GINT_TO_POINTER (-1));
	    g_strfreev ( tab_char );
	}
    }
    else
	/* no party so not a report */
	parties_list = g_slist_append ( parties_list,
					GINT_TO_POINTER (-1));
    return parties_list;
}



/**
 * validate a transfert from a form :
 * - create the contra-transaction
 * - delete the last contra-transaction if it's a modification
 * - append the contra-transaction to the tree view or update the tree_view
 * 
 * \param transaction_number the new transaction or the modify transaction
 * \param new_transaction TRUE if it's a new transaction
 * \param account_transfer the number of the account we want to create the contra-transaction
 * 
 * \return the number of the contra-transaction
 * */
gint gsb_form_transaction_validate_transfer ( gint transaction_number,
					      gint new_transaction,
					      gint account_transfer )
{
    gint contra_transaction_number;
    gint contra_mother_number = 0;

    g_return_val_if_fail ( account_transfer, -1 );

    /* either it's a new transfer or a change of a non-transfer transaction
     * either it was already a transfer, in that case, if we change the target account,
     * we delete the contra-transaction and it's the same as a new transfer */
    if ( !new_transaction )
    {
	/* it's a modification of a transaction */

	/* as we will do a transfer, the category number is null */
	gsb_data_transaction_set_category_number ( transaction_number,
						   0 );
	gsb_data_transaction_set_sub_category_number ( transaction_number,
						       0 );

	if ((contra_transaction_number = gsb_data_transaction_get_contra_transaction_number (transaction_number)))
	{
	    /* the transaction is a transfer */

	    /* if the contra transaction was a child of breakdown, copying/deleting it will remove the information
	     * of the mother, so we get it here */
	    contra_mother_number = gsb_data_transaction_get_mother_transaction_number (contra_transaction_number);

	    /* check if we change the account targe */
	    if ( gsb_data_transaction_get_contra_transaction_account (transaction_number) != account_transfer )
	    {
		/* it was a transfer and the user changed the target account so we delete the last contra transaction
		 * contra_transaction_transfer has just been set */

		gsb_data_transaction_set_contra_transaction_number ( contra_transaction_number,
								       0);
		gsb_transactions_list_delete_transaction (contra_transaction_number, FALSE);
		new_transaction = 1;
	    }
	}
	else
	{
	    /* the transaction was not a transfer, so it's the same as a new transaction, to do the contra-transaction */

	    new_transaction = 1;
	}
    }

    /* so, now, it's either a new transfer and new_transaction is TRUE,
     * either a transfer without changing the target account and in that case, contra_transaction_number is
     * already set */

    if ( new_transaction )
	contra_transaction_number = gsb_data_transaction_new_transaction (account_transfer);

    gsb_data_transaction_copy_transaction ( transaction_number,
					    contra_transaction_number );

    /* contra_mother_number contains the mother number of the contra transaction if it was a modification
     * and that contra-transaction was a child of breakdown, and if not it is 0, and it's a good thing because
     * now contra-transaction has a mother number if the transaction was a child of breakdown, and we have to
     * set that mother number to 0 (for the contra-transaction) */
    gsb_data_transaction_set_mother_transaction_number ( contra_transaction_number,
							 contra_mother_number );

    /* we have to change the amount by the opposite */
    gsb_data_transaction_set_amount (contra_transaction_number,
				     gsb_real_opposite (gsb_data_transaction_get_amount (transaction_number)));;


    /* we have to check the change */
    gsb_currency_check_for_change ( contra_transaction_number );

    /* set the method of payment
     * FIXME : for the scheduled transactions, we arrive here but there is no button for that,
     * so for now, TRANSACTION_FORM_CONTRA won't be visible so he just copy the method of the scheduled transaction */

    if ( gsb_form_widget_get_widget (TRANSACTION_FORM_CONTRA)
	 &&
	 GTK_IS_WIDGET ( gsb_form_widget_get_widget (TRANSACTION_FORM_CONTRA))
	 &&
	 gsb_data_form_check_for_value ( TRANSACTION_FORM_CONTRA )
	 &&
	 GTK_WIDGET_VISIBLE ( gsb_form_widget_get_widget (TRANSACTION_FORM_CONTRA)))
    {
	gsb_data_transaction_set_method_of_payment_number ( contra_transaction_number,
							    gsb_payment_method_get_selected_number (gsb_form_widget_get_widget (TRANSACTION_FORM_CONTRA)));
    }

    /* set the link between the transactions */
    gsb_data_transaction_set_contra_transaction_number ( transaction_number,
							   contra_transaction_number);
    gsb_data_transaction_set_contra_transaction_account ( transaction_number,
						       gsb_data_transaction_get_account_number (contra_transaction_number));
    gsb_data_transaction_set_contra_transaction_number ( contra_transaction_number,
							   transaction_number);
    gsb_data_transaction_set_contra_transaction_account ( contra_transaction_number,
						       gsb_data_transaction_get_account_number (transaction_number));

    /* show the contra_transaction */
    if ( new_transaction )
	gsb_transactions_list_append_new_transaction (contra_transaction_number, TRUE);
    else
	gsb_transactions_list_update_transaction (contra_transaction_number);

    return contra_transaction_number;
}



/**
 * callback called when the user click on the 'change' button in the form
 * show a popup to modify the values
 *
 * \param button
 * \param null
 *
 * \return FALSE
 * */
gboolean gsb_form_transaction_change_clicked ( GtkWidget *button,
					       gpointer null )
{
    gint transaction_number;
    gint account_number;
    gint currency_number;
    gint account_currency_number;
    gsb_real exchange, exchange_fees;

    account_number = gsb_form_get_account_number ();
    gtk_widget_grab_focus ( gsb_form_widget_get_widget (TRANSACTION_FORM_DATE));

    transaction_number = GPOINTER_TO_INT (gtk_object_get_data ( GTK_OBJECT ( gsb_form_get_form_widget () ),
								"transaction_number_in_form" ));

    account_currency_number = gsb_data_account_get_currency (account_number);
    currency_number = gsb_data_transaction_get_currency_number (transaction_number);

    gsb_currency_exchange_dialog ( account_currency_number, currency_number,
				   gsb_data_transaction_get_change_between (transaction_number),
				   gsb_data_transaction_get_exchange_rate (transaction_number),
				   gsb_data_transaction_get_exchange_fees (transaction_number), 
				   TRUE );

    exchange = gsb_currency_get_current_exchange ();
    exchange_fees = gsb_currency_get_current_exchange_fees ();

    if ( exchange.mantissa || exchange_fees.mantissa )
    {
	gsb_data_transaction_set_exchange_rate (transaction_number,
						 gsb_real_abs (exchange));
	gsb_data_transaction_set_exchange_fees (transaction_number,
						exchange_fees );

	if ( exchange.mantissa < 0 )
	    gsb_data_transaction_set_change_between (transaction_number,
						     1 );
	else
	    gsb_data_transaction_set_change_between (transaction_number,
						      0 );
    }
    return FALSE;
}


/* Local Variables: */
/* c-basic-offset: 4 */
/* End: */
