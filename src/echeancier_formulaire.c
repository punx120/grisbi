/* ************************************************************************** */
/* Ce fichier s'occupe de la gestion du formulaire de saisie des �ch�ances    */
/*			echeances_formulaire.c                                */
/*                                                                            */
/*     Copyright (C)	2000-2003 C�dric Auger (cedric@grisbi.org)	      */
/*			2003-2004 Alain Portal (dionysos@grisbi.org)	      */
/*			http://www.grisbi.org				      */
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


#include "include.h"
#include "structures.h"
#include "variables-extern.c"
#include "echeancier_formulaire.h"



#include "accueil.h"
#include "calendar.h"
#include "categories_onglet.h"
#include "comptes_traitements.h"
#include "constants.h"
#include "devises.h"
#include "dialog.h"
#include "echeancier_liste.h"
#include "exercice.h"
#include "imputation_budgetaire.h"
#include "operations_formulaire.h"
#include "operations_liste.h"
#include "search_glist.h"
#include "tiers_onglet.h"
#include "traitement_variables.h"
#include "type_operations.h"
#include "utils.h"



# define SCHEDULER_FORM_DATE 0
# define SCHEDULER_FORM_PARTY 1
# define SCHEDULER_FORM_DEBIT 2
# define SCHEDULER_FORM_CREDIT 3
# define SCHEDULER_FORM_DEVISE 4
# define SCHEDULER_FORM_ACCOUNT 5
# define SCHEDULER_FORM_CATEGORY 6
# define SCHEDULER_FORM_TYPE 7
# define SCHEDULER_FORM_CHEQUE 8
# define SCHEDULER_FORM_EXERCICE 9
# define SCHEDULER_FORM_BUDGETARY 10
# define SCHEDULER_FORM_BANK 11
# define SCHEDULER_FORM_VOUCHER 12
# define SCHEDULER_FORM_MODE 13
# define SCHEDULER_FORM_NOTES 14
# define SCHEDULER_FORM_FREQUENCY 15
# define SCHEDULER_FORM_FINAL_DATE 16
# define SCHEDULER_FORM_FREQ_CUSTOM_NB 17
# define SCHEDULER_FORM_FREQ_CUSTOM_MENU 18
# define SCHEDULER_FORM_TOTAL_WIDGET 19	 /* must be the last of the list */
/*
# define SCHEDULER_FORM_CHANGE
# define SCHEDULER_FORM_CONTRA
# define SCHEDULER_FORM_BREAKDOWN
*/

/******************************************************************************/
/*  Routine qui cr�e le formulaire et le renvoie */
/******************************************************************************/
GtkWidget *creation_formulaire_echeancier ( void )
{
    GtkWidget *menu;
    GtkWidget *item;
    GtkTooltips *tips;
    GtkWidget *bouton;
    GtkWidget *table;


    /* on cr�e le tooltips */

    tips = gtk_tooltips_new ();


    formulaire_echeancier = gtk_vbox_new ( FALSE,
					   5 );
    gtk_widget_show ( formulaire_echeancier );


    /*   cr�ation de la table */

    table = gtk_table_new ( 6,
			    4,
			    FALSE );
    gtk_container_set_border_width ( GTK_CONTAINER ( table ),
				     10 );
    gtk_table_set_col_spacings ( GTK_TABLE ( table ),
				 5 );
    gtk_widget_set_usize ( table,
			   1,
			   FALSE );
    gtk_box_pack_start ( GTK_BOX ( formulaire_echeancier ),
			 table,
			 TRUE,
			 TRUE,
			 0 );
    gtk_widget_show ( table );



    /* cr�ation du label saisie / modif */

    label_saisie_modif = gtk_label_new ( "" );
    gtk_table_attach ( GTK_TABLE ( table ),
		       label_saisie_modif,
		       0, 1,
		       0, 1,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0,0);


    /* cr�ation de l'entr�e de la date */

    widget_formulaire_echeancier[SCHEDULER_FORM_DATE] = gtk_entry_new_with_max_length ( 10 );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_DATE],
		       1, 2,
		       0, 1,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_DATE ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE] ),
			 "focus-in-event",
			 GTK_SIGNAL_FUNC ( entree_prend_focus ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE] ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus_echeancier ),
			 NULL );
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE] );

    /* cr�ation du combofix des tiers */

    widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] = gtk_combofix_new (  liste_tiers_combofix_echeancier,
									     FALSE,
									     TRUE,
									     TRUE,
									     0 );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] ) -> entry ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_PARTY ) );
    gtk_signal_connect_object ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY]) -> entry),
				"focus-in-event",
				GTK_SIGNAL_FUNC ( entree_prend_focus ),
				GTK_WIDGET ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] ) -> entry ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_PARTY ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] ) -> entry ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_PARTY ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] ) -> arrow ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_PARTY ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_PARTY],
		       2, 3,
		       0, 1,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] );

    /* cr�ation de l'entr�e des d�bits */

    widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] = gtk_entry_new ();
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_DEBIT ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_DEBIT ) );
    gtk_signal_connect ( GTK_OBJECT (widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT]),
			 "focus-in-event",
			 GTK_SIGNAL_FUNC ( entree_prend_focus ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_DEBIT ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT],
		       3, 4,
		       0, 1,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] );


    /* cr�ation de l'entr�e des cr�dits */

    widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] = gtk_entry_new ();
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_CREDIT ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_CREDIT ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ),
			 "focus-in-event",
			 GTK_SIGNAL_FUNC ( entree_prend_focus ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_CREDIT ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT],
		       4, 5,
		       0, 1,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] );


    /* met l'option menu des devises */

    widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE] = gtk_option_menu_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE],
			   _("Choose currency"),
			   _("Choose currency") );
    menu = creation_option_menu_devises ( -1,
					  liste_struct_devises );
    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE] ),
			       menu );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_DEVISE ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE],
		       5, 7,
		       0, 1,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE] );

    /* Mise en place du menu des comptes */

    widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] = gtk_option_menu_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT],
			   _("Choose the account"),
			   _("Choose the account") );

    menu = creation_option_menu_comptes ( GTK_SIGNAL_FUNC(changement_choix_compte_echeancier), TRUE );
    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ),
			       menu );
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ),
				  0);

    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_ACCOUNT ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT],
		       0, 2,
		       1, 2,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] );

    /* Affiche les cat�gories / sous-cat�gories */

    widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] = gtk_combofix_new_complex ( liste_categories_echeances_combofix,
										       FALSE,
										       TRUE,
										       TRUE,
										       0 );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ) -> entry ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_CATEGORY ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ) -> entry ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_CATEGORY ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ) -> arrow ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_CATEGORY ) );
    gtk_signal_connect_object ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ) -> entry ),
				"focus-in-event",
				GTK_SIGNAL_FUNC ( entree_prend_focus ),
				widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ) -> entry ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_CATEGORY ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY],
		       2, 3,
		       1, 2,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] );

    /* Cr�ation de l'entr�e du ch�que, non affich�e pour le moment
       � cr�er avant l'option menu du type d'op� */

    widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] = gtk_entry_new();
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER( SCHEDULER_FORM_CHEQUE ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_CHEQUE ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] ),
			 "focus-in-event",
			 GTK_SIGNAL_FUNC ( entree_prend_focus ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_CHEQUE ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE],
		       5, 7,
		       1, 2,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] );

    /*  Affiche l'option menu des types */

    widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] = gtk_option_menu_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_echeancier[SCHEDULER_FORM_TYPE],
			   _("Choose the method of payment"),
			   _("Choose the method of payment") );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_TYPE ) );
    gtk_table_attach ( GTK_TABLE (table),
		       widget_formulaire_echeancier[SCHEDULER_FORM_TYPE],
		       3, 5,
		       1, 2,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);

    /* le menu par d�faut est celui des d�bits */

    if ( ( menu = creation_menu_types ( 1,
					GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ) -> menu_item ),
										"no_compte" )),
					1 )))
    {
	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ) -> menu_item ),
												     "no_compte" ));

	gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
				   menu );
	gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
				      cherche_no_menu_type_echeancier ( TYPE_DEFAUT_DEBIT ) );
	gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] );
    }

    /* met l'option menu de l'exercice */

    widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] = gtk_option_menu_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE],
			   _("Choose the financial year"),
			   _("Choose the financial year") );
    menu = gtk_menu_new ();
    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] ),
			       creation_menu_exercices (1) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER( SCHEDULER_FORM_EXERCICE ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE],
		       0, 2,
		       2, 3,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] );

    gtk_widget_set_sensitive ( widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE],
			       etat.utilise_exercice );

    /*  Affiche l'imputation budg�taire */

    widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] = gtk_combofix_new_complex ( liste_imputations_combofix,
											FALSE,
											TRUE,
											TRUE ,
											0);
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY],
		       2, 3,
		       2, 3,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ) -> entry),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER( SCHEDULER_FORM_BUDGETARY ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ) -> entry ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_BUDGETARY ) );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ) -> arrow ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_BUDGETARY ) );
    gtk_signal_connect_object ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ) -> entry ),
				"focus-in-event",
				GTK_SIGNAL_FUNC ( entree_prend_focus ),
				widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] );
    gtk_signal_connect ( GTK_OBJECT ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ) -> entry ),
			 "focus-out-event",
			 GTK_SIGNAL_FUNC ( entree_perd_focus_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_BUDGETARY ) );

    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] );

    gtk_widget_set_sensitive ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY],
			       etat.utilise_imputation_budgetaire );

    /* Affiche les infos banque/guichet
       � ne pas mettre, mais on r�serve encore le widget n�
       ne pas l'effacer pour respecter les tabulations */

    widget_formulaire_echeancier[SCHEDULER_FORM_BANK] = gtk_entry_new ();
    /*
       gtk_table_attach ( GTK_TABLE ( table ),
       widget_formulaire_echeancier[SCHEDULER_FORM_BANK],
       3, 5, 2, 3,
       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
       GTK_SHRINK | GTK_FILL,
       0, 0);
       gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_BANK] ),
       "button-press-event",
       GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
       GINT_TO_POINTER ( SCHEDULER_FORM_BANK ) );
       gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_BANK]),
       "key-press-event",
       GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
       GINT_TO_POINTER ( SCHEDULER_FORM_BANK ) );
       gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_BANK]),
       "focus-in-event",
       GTK_SIGNAL_FUNC ( entree_prend_focus ),
       NULL );
       gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_BANK]),
       "focus-out-event",
       GTK_SIGNAL_FUNC ( entree_perd_focus_echeancier ),
       GINT_TO_POINTER ( SCHEDULER_FORM_BANK ) );
       if ( etat.utilise_info_banque_guichet )
       gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_BANK]);
       */

    /* cr�ation de l'entr�e du no de pi�ce comptable
       � ne pas mettre, mais on r�serve encore le widget n�
       ne pas l'effacer pour respecter les tabulations */

    widget_formulaire_echeancier[SCHEDULER_FORM_VOUCHER] = gtk_entry_new();

    /*
       gtk_table_attach ( GTK_TABLE ( table ),
       widget_formulaire_echeancier[SCHEDULER_FORM_VOUCHER],
       5, 7, 2, 3,
       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
       GTK_SHRINK | GTK_FILL,
       0, 0);
       gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_VOUCHER] ),
       "button-press-event",
       GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
       GINT_TO_POINTER ( SCHEDULER_FORM_VOUCHER ) );
       gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_VOUCHER]),
       "key-press-event",
       GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
       GINT_TO_POINTER ( SCHEDULER_FORM_VOUCHER ) );
       gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_VOUCHER]),
       "focus-in-event",
       GTK_SIGNAL_FUNC ( entree_prend_focus ),
       NULL );
       gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_VOUCHER]),
       "focus-out-event",
       GTK_SIGNAL_FUNC ( entree_perd_focus_echeancier ),
       GINT_TO_POINTER ( SCHEDULER_FORM_VOUCHER ) );

       if ( etat.utilise_piece_comptable )
       gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_VOUCHER] );
       */

    /* Mise en place du menu automatique/manuel */

    widget_formulaire_echeancier[SCHEDULER_FORM_MODE] = gtk_option_menu_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_echeancier[SCHEDULER_FORM_MODE],
			   _("Automatic/manual scheduled transaction"),
			   _("Automatic/manual scheduled transaction") );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_MODE ) );

    menu = gtk_menu_new ();

    item = gtk_menu_item_new_with_label ( _("Manual"));
    gtk_object_set_data ( GTK_OBJECT ( item ),
			  "auto_man",
			  GINT_TO_POINTER ( 0 ));
    gtk_menu_append ( GTK_MENU ( menu ),
		      item );
    gtk_widget_show ( item );

    item = gtk_menu_item_new_with_label ( _("Automatic"));
    gtk_object_set_data ( GTK_OBJECT ( item ),
			  "auto_man",
			  GINT_TO_POINTER ( 1 ));
    gtk_menu_append ( GTK_MENU ( menu ),
		      item );
    gtk_widget_show ( item );

    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE] ),
			       menu );

    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE] ),
				  0);
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_MODE],
		       0, 2,
		       3, 4,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE] );

    /*  Affiche les notes */

    widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] = gtk_entry_new ();
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_NOTES ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_NOTES ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] ),
			 "focus-in-event",
			 GTK_SIGNAL_FUNC ( entree_prend_focus ),
			 NULL );
    gtk_signal_connect_after ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] ),
			       "focus-out-event",
			       GTK_SIGNAL_FUNC ( entree_perd_focus_echeancier ),
			       GINT_TO_POINTER ( SCHEDULER_FORM_NOTES ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_NOTES],
		       2, 3,
		       3, 4,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] );

    /* Mise en place du menu de la fr�quence */

    widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY] = gtk_option_menu_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY],
			   _("Frequency"),
			   _("Frequency") );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_FREQUENCY ) );

    menu = gtk_menu_new ();

    item = gtk_menu_item_new_with_label ( _("Once"));
    gtk_object_set_data ( GTK_OBJECT  ( item ),
			  "periodicite",
			  GINT_TO_POINTER ( 0 ));
    gtk_menu_append ( GTK_MENU ( menu ),
		      item );
    gtk_signal_connect ( GTK_OBJECT  ( item ),
			 "activate",
			 GTK_SIGNAL_FUNC ( cache_date_limite_echeancier ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT  ( item ),
			 "activate",
			 GTK_SIGNAL_FUNC ( cache_personnalisation_echeancier ),
			 NULL );
    gtk_widget_show ( item );

    item = gtk_menu_item_new_with_label ( _("Weekly"));
    gtk_object_set_data ( GTK_OBJECT  ( item ),
			  "periodicite",
			  GINT_TO_POINTER ( 1 ));
    gtk_menu_append ( GTK_MENU ( menu ),
		      item );
    gtk_signal_connect ( GTK_OBJECT  ( item ),
			 "activate",
			 GTK_SIGNAL_FUNC ( affiche_date_limite_echeancier ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT  ( item ),
			 "activate",
			 GTK_SIGNAL_FUNC ( cache_personnalisation_echeancier ),
			 NULL );
    gtk_widget_show ( item );

    item = gtk_menu_item_new_with_label ( _("Monthly"));
    gtk_object_set_data ( GTK_OBJECT  ( item ),
			  "periodicite",
			  GINT_TO_POINTER ( 2 ));
    gtk_menu_append ( GTK_MENU ( menu ),
		      item );
    gtk_signal_connect ( GTK_OBJECT  ( item ),
			 "activate",
			 GTK_SIGNAL_FUNC ( affiche_date_limite_echeancier ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT  ( item ),
			 "activate",
			 GTK_SIGNAL_FUNC ( cache_personnalisation_echeancier ),
			 NULL );
    gtk_widget_show ( item );

    item = gtk_menu_item_new_with_label ( _("Yearly"));
    gtk_object_set_data ( GTK_OBJECT  ( item ),
			  "periodicite",
			  GINT_TO_POINTER ( 3 ));
    gtk_menu_append ( GTK_MENU ( menu ),
		      item );
    gtk_signal_connect ( GTK_OBJECT  ( item ),
			 "activate",
			 GTK_SIGNAL_FUNC ( affiche_date_limite_echeancier ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT  ( item ),
			 "activate",
			 GTK_SIGNAL_FUNC ( cache_personnalisation_echeancier ),
			 NULL );
    gtk_widget_show ( item );

    item = gtk_menu_item_new_with_label ( _("Custom"));
    gtk_object_set_data ( GTK_OBJECT  ( item ),
			  "periodicite",
			  GINT_TO_POINTER ( 4 ));
    gtk_menu_append ( GTK_MENU ( menu ),
		      item );
    gtk_signal_connect ( GTK_OBJECT  ( item ),
			 "activate",
			 GTK_SIGNAL_FUNC ( affiche_date_limite_echeancier ),
			 NULL );
    gtk_signal_connect ( GTK_OBJECT  ( item ),
			 "activate",
			 GTK_SIGNAL_FUNC ( affiche_personnalisation_echeancier ),
			 NULL );
    gtk_widget_show ( item );

    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY] ),
			       menu );

    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY] ),
				  0);

    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY],
		       3, 4,
		       3, 4,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY] );

    /* entr�e de la date limite, non affich�e au d�part */

    widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] = gtk_entry_new_with_max_length (11);
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_FINAL_DATE ) );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] ),
			 "button-press-event",
			 GTK_SIGNAL_FUNC ( clique_champ_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_FINAL_DATE ) );
    gtk_signal_connect ( GTK_OBJECT (widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE]),
			 "focus-in-event",
			 GTK_SIGNAL_FUNC ( entree_prend_focus ),
			 NULL );
    gtk_signal_connect_after ( GTK_OBJECT (widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE]),
			       "focus-out-event",
			       GTK_SIGNAL_FUNC ( entree_perd_focus_echeancier ),
			       GINT_TO_POINTER ( SCHEDULER_FORM_FINAL_DATE ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE],
		       4, 5,
		       3, 4,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] );

    /* et la gtk entry quand la fr�quence est personnalis�e, non affich� au d�part */

    widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_NB] = gtk_entry_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_NB],
			   _("Custom frequency"),
			   _("Custom frequency") );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_NB] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_FREQ_CUSTOM_NB ) );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_NB],
		       5, 6,
		       3, 4,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_NB] );

    /* le menu jour / mois / ann�e */

    widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU] = gtk_option_menu_new ();
    gtk_tooltips_set_tip ( GTK_TOOLTIPS ( tips ),
			   widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU],
			   _("Custom frequency"),
			   _("Custom frequency") );
    gtk_signal_connect ( GTK_OBJECT ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU] ),
			 "key-press-event",
			 GTK_SIGNAL_FUNC ( pression_touche_formulaire_echeancier ),
			 GINT_TO_POINTER ( SCHEDULER_FORM_FREQ_CUSTOM_MENU ) );

    menu = gtk_menu_new ();

    item = gtk_menu_item_new_with_label ( _("Days") );
    gtk_object_set_data ( GTK_OBJECT ( item ),
			  "intervalle_perso",
			  GINT_TO_POINTER ( 0 ));
    gtk_menu_append ( GTK_MENU ( menu ),
		      item );
    gtk_widget_show ( item );

    item = gtk_menu_item_new_with_label ( _("Months") );
    gtk_object_set_data ( GTK_OBJECT ( item ),
			  "intervalle_perso",
			  GINT_TO_POINTER ( 1 ));
    gtk_menu_append ( GTK_MENU ( menu ),
		      item );
    gtk_widget_show ( item );

    item = gtk_menu_item_new_with_label ( _("Years") );
    gtk_object_set_data ( GTK_OBJECT ( item ),
			  "intervalle_perso",
			  GINT_TO_POINTER ( 2 ));
    gtk_menu_append ( GTK_MENU ( menu ),
		      item );
    gtk_widget_show ( item );

    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU] ),
			       menu );
    gtk_table_attach ( GTK_TABLE ( table ),
		       widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU],
		       6, 7,
		       3, 4,
		       GTK_SHRINK | GTK_FILL | GTK_EXPAND,
		       GTK_SHRINK | GTK_FILL,
		       0, 0);
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU] );

    /* s�paration d'avec les boutons */

    separateur_formulaire_echeancier = gtk_hseparator_new ();
    gtk_box_pack_start ( GTK_BOX ( formulaire_echeancier ),
			 separateur_formulaire_echeancier,
			 FALSE,
			 FALSE,
			 0 );
    if ( etat.affiche_boutons_valider_annuler )
	gtk_widget_show ( separateur_formulaire_echeancier );

    /* mise en place des boutons */

    hbox_valider_annuler_echeance = gtk_hbox_new ( FALSE,
						   5 );
    gtk_box_pack_start ( GTK_BOX ( formulaire_echeancier ),
			 hbox_valider_annuler_echeance,
			 FALSE,
			 FALSE,
			 0 );
    if ( etat.affiche_boutons_valider_annuler )
	gtk_widget_show ( hbox_valider_annuler_echeance );

    bouton = gtk_button_new_from_stock ( GTK_STOCK_OK );
    gtk_button_set_relief ( GTK_BUTTON ( bouton ), GTK_RELIEF_NONE );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( fin_edition_echeance ),
			 NULL );
    gtk_box_pack_end ( GTK_BOX ( hbox_valider_annuler_echeance ),
		       bouton,
		       FALSE,
		       FALSE,
		       0 );
    gtk_widget_show ( bouton );

    bouton = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
    gtk_button_set_relief ( GTK_BUTTON ( bouton ), GTK_RELIEF_NONE );
    gtk_signal_connect ( GTK_OBJECT ( bouton ),
			 "clicked",
			 GTK_SIGNAL_FUNC ( echap_formulaire_echeancier ),
			 NULL );
    gtk_box_pack_end ( GTK_BOX ( hbox_valider_annuler_echeance ),
		       bouton,
		       FALSE,
		       FALSE,
		       0 );
    gtk_widget_show ( bouton );

    /* on associe au formulaire l'adr de l'�ch�ance courante */

    gtk_object_set_data ( GTK_OBJECT ( formulaire_echeancier ),
			  "adr_echeance",
			  NULL );

    formulaire_echeancier_a_zero ();

    return ( formulaire_echeancier);
}
/******************************************************************************/

/******************************************************************************/
void echap_formulaire_echeancier ( void )
{
    formulaire_echeancier_a_zero();

    gtk_widget_grab_focus ( liste_echeances );

    if ( !etat.formulaire_echeancier_toujours_affiche )
	gtk_widget_hide ( frame_formulaire_echeancier );
}
/******************************************************************************/

/******************************************************************************/
/* Fonction appel�e quand une entry perd le focus */
/* si elle ne contient rien, on remet la fonction en gris */
/******************************************************************************/
gboolean entree_perd_focus_echeancier ( GtkWidget *entree,
					GdkEventFocus *ev,
					gint *no_origine )
{
    gchar *texte;
    texte = NULL;

    /* !!!!!!!mettre p_tab... en fonction du compte s�lectionn� */


    switch ( GPOINTER_TO_INT ( no_origine ))
    {
	/* on sort de la date, soit c'est vide, soit on la v�rifie, la compl�te si n�cessaire et met � jour l'exercice */
	case SCHEDULER_FORM_DATE :
	    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
	    {
		modifie_date ( entree );

		/* on ne change l'exercice que si c'est une nouvelle �ch�ance */

		if ( !gtk_object_get_data ( GTK_OBJECT ( formulaire_echeancier ),
					    "adr_echeance" ))
		    affiche_exercice_par_date( widget_formulaire_echeancier[SCHEDULER_FORM_DATE],
					       widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] );
	    }
	    else
		texte = _("Date");
	    break;

	    /* on sort du tiers : soit vide soit compl�te le reste de l'op� */

	case SCHEDULER_FORM_PARTY :
	    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		completion_operation_par_tiers_echeancier ();
	    else
		texte = _("Third party");
	    break;

	    /* on sort du d�bit : soit vide, soit change le menu des types
	       s'il ne correspond pas */

	case SCHEDULER_FORM_DEBIT :

	    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
	    {
		/* on commence par virer ce qu'il y avait dans les cr�dits */

		if ( gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ) == style_entree_formulaire[ENCLAIR] )
		{
		    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ),
					 "" );
		    entree_perd_focus_echeancier ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT],
						   NULL,
						   GINT_TO_POINTER ( SCHEDULER_FORM_CREDIT ));
		}

		if ( GTK_WIDGET_VISIBLE ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) &&
		     GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) -> menu ),
							     "signe_menu" )) == 2 )
		{
		    if ( etat.affiche_tous_les_types )
		    {
			/* on ne modifie que le d�faut */

			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ) -> menu_item ),
														     "no_compte" ));
			gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
						      cherche_no_menu_type_echeancier ( TYPE_DEFAUT_DEBIT ) );
			gtk_object_set_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) -> menu ),
					      "signe_menu",
					      GINT_TO_POINTER ( 1 ));
		    }
		    else
		    {
			GtkWidget *menu;

			if ( ( menu = creation_menu_types ( 1,
							    GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ) -> menu_item ),
												    "no_compte" )),
							    1 )))
			{
			    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ) -> menu_item ),
															 "no_compte" ));

			    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
						       menu );
			    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
							  cherche_no_menu_type_echeancier ( TYPE_DEFAUT_DEBIT ) );
			    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] );
			}
			else
			    gtk_widget_hide ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] );
		    }	     
		}
	    }
	    else
		texte = _("Debit");
	    break;

	    /* on sort du cr�dit : soit vide, soit change le menu des types
	       s'il n'y a aucun tiers ( <=> nouveau tiers ) */

	case SCHEDULER_FORM_CREDIT :
	    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
	    {
		/* on commence par virer ce qu'il y avait dans les d�bits */

		if ( gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ) == style_entree_formulaire[ENCLAIR] )
		{
		    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ),
					 "" );
		    entree_perd_focus_echeancier ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT],
						   NULL,
						   GINT_TO_POINTER ( SCHEDULER_FORM_DEBIT ));
		}


		if ( GTK_WIDGET_VISIBLE ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) &&
		     GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) -> menu ),
							     "signe_menu" )) == 1 )
		{
		    if ( etat.affiche_tous_les_types )
			/* on ne modifie que le d�faut */
		    {
			p_tab_nom_de_compte_variable = p_tab_nom_de_compte + GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ) -> menu_item ),
														     "no_compte" ));

			gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
						      cherche_no_menu_type_echeancier ( TYPE_DEFAUT_CREDIT ) );
			gtk_object_set_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) -> menu ),
					      "signe_menu",
					      GINT_TO_POINTER ( 2 ));
		    }

		    else
		    {
			GtkWidget *menu;

			if ( ( menu = creation_menu_types ( 2,
							    GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ) -> menu_item ),
												    "no_compte" )),
							    1  )))
			{
			    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ) -> menu_item ),
															 "no_compte" ));

			    gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
						       menu );
			    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
							  cherche_no_menu_type_echeancier ( TYPE_DEFAUT_CREDIT ) );
			    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] );
			}
			else
			    gtk_widget_hide ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] );
		    }
		}
	    }
	    else
		texte = _("Credit");
	    break;

	    /*       sort des cat�gories */

	case SCHEDULER_FORM_CATEGORY :
	    if ( !strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		texte = _("Categories : Sub-categories");

	    break;

	case SCHEDULER_FORM_CHEQUE :
	    if ( !strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		texte = _("Transfer reference");
	    break;

	case SCHEDULER_FORM_BUDGETARY :
	    if ( !strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		texte = _("Budgetary line");
	    break;

	case SCHEDULER_FORM_BANK :
	    if ( !strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		texte = _("Bank references");
	    break;

	case SCHEDULER_FORM_VOUCHER :
	    if ( !strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		texte = _("Voucher");

	    break;

	case SCHEDULER_FORM_NOTES :
	    if ( !strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		texte = _("Notes");
	    break;

	    /* on sort de la date limite, soit c'est vide, soit on la v�rifie,
	       la compl�te si n�cessaire */
	case SCHEDULER_FORM_FINAL_DATE :
	    if ( strlen ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( entree )))))
		modifie_date ( entree );
	    else
		texte = _("Limit date");
	    break;
    }

    /* l'entr�e �tait vide, on remet le d�faut
       si l'origine �tait un combofix, il faut remettre le texte 
       avec le gtk_combofix (sinon risque de compl�tion), donc utiliser l'origine */

    if ( texte )
    {
	switch ( GPOINTER_TO_INT ( no_origine ) )
	{
	    case SCHEDULER_FORM_PARTY :
	    case SCHEDULER_FORM_CATEGORY :
	    case SCHEDULER_FORM_BUDGETARY :
		gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[GPOINTER_TO_INT ( no_origine )] ),
					texte );
		break;

	    default:
		gtk_entry_set_text ( GTK_ENTRY ( entree ), texte );
		break;
	}
	gtk_widget_set_style ( entree, style_entree_formulaire[ENGRIS] );
    }

    return FALSE;
}
/******************************************************************************/

/******************************************************************************/
void affiche_cache_le_formulaire_echeancier ( void )
{
    if ( etat.formulaire_echeancier_toujours_affiche )
    {
	gtk_widget_hide ( fleche_bas_echeancier );
	gtk_widget_show ( fleche_haut_echeancier );

	gtk_widget_hide ( frame_formulaire_echeancier );
	etat.formulaire_echeancier_toujours_affiche = 0;
    }
    else
    {
	gtk_widget_hide ( fleche_haut_echeancier );
	gtk_widget_show ( fleche_bas_echeancier );

	gtk_widget_show ( frame_formulaire_echeancier );
	etat.formulaire_echeancier_toujours_affiche = 1;
    }

    gtk_widget_grab_focus ( liste_echeances );
}
/******************************************************************************/

/******************************************************************************/
gboolean clique_champ_formulaire_echeancier ( GtkWidget *entree,
					      GdkEventButton *ev,
					      gint *no_origine )
{
    GtkWidget *popup_cal;
    /* on rend sensitif tout ce qui ne l'�tait pas sur le formulaire */

    degrise_formulaire_echeancier ();

    /* si l'entr�e de la date et grise, on met la date courante */

    if ( gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE] ) == style_entree_formulaire[ENGRIS] )
    {
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE] ),
			     gsb_today() );

	gtk_widget_set_style ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE],
			       style_entree_formulaire[ENCLAIR] );
    }

    /* si ev est null ( c�d que �a ne vient pas d'un click mais appel� par ex
       � la fin de fin_edition ), on se barre */

    if ( !ev )
	return FALSE;

    /* �num�ration suivant l'entr�e o� on clique */

    switch ( GPOINTER_TO_INT ( no_origine ) )
    {
	case SCHEDULER_FORM_DATE :
	case SCHEDULER_FORM_FINAL_DATE :
	    /* click sur l'entr�e de la date ou date limite */

	    /* si double click, on popup le calendrier */

	    if ( ev -> type == GDK_2BUTTON_PRESS )
	    {
		gtk_signal_emit_stop_by_name ( GTK_OBJECT ( entree ),
					       "button-press-event");
		popup_cal = gsb_calendar_new ( entree );
		gtk_signal_connect_object ( GTK_OBJECT ( popup_cal ),
					    "destroy",
					    GTK_SIGNAL_FUNC ( ferme_calendrier ),
					    GTK_OBJECT ( entree ) );
		gtk_widget_grab_focus ( GTK_WIDGET ( popup_cal ) );
	    }
	    break;

	default :

	    break;
    }
    return FALSE;
}
/******************************************************************************/

/******************************************************************************/
gboolean pression_touche_formulaire_echeancier ( GtkWidget *widget,
						 GdkEventKey *ev,
						 gint no_widget )
{
    GtkWidget *popup_cal;

    /* si etat.entree = 1, la touche entr�e finit l'op�ration ( fonction par d�faut ) */
    /* sinon elle fait comme tab */

    if ( !etat.entree && ( ev -> keyval == GDK_Return || ev -> keyval == GDK_KP_Enter ))
	ev->keyval = GDK_Tab ;


    switch ( ev -> keyval )
    {

	case GDK_Escape :		/* �chap */

	    echap_formulaire_echeancier();
	    return FALSE;

	case GDK_Tab :

	    if ( GTK_IS_ENTRY ( widget ) )
		gtk_entry_select_region ( GTK_ENTRY ( widget ), 0, 0);
	    else
		if ( GTK_IS_COMBOFIX ( widget ) )
		    gtk_entry_select_region ( GTK_ENTRY ( GTK_COMBOFIX ( widget ) -> entry ), 0, 0);

	    /* on fait perdre le focus au widget courant pour faire
	       les changements automatiques si n�cessaire */
	    gtk_widget_grab_focus ( liste_echeances );

	    /* on donne le focus au widget suivant */
	    no_widget = ( no_widget + 1 ) % SCHEDULER_FORM_TOTAL_WIDGET;

	    while ( !( GTK_WIDGET_VISIBLE ( widget_formulaire_echeancier[no_widget] ) &&
		       GTK_WIDGET_SENSITIVE ( widget_formulaire_echeancier[no_widget] ) &&
		       ( GTK_IS_COMBOFIX ( widget_formulaire_echeancier[no_widget] ) ||
			 GTK_IS_ENTRY ( widget_formulaire_echeancier[no_widget] ) ||
			 GTK_IS_BUTTON ( widget_formulaire_echeancier[no_widget] ) )))
		no_widget = ( no_widget + 1 ) % SCHEDULER_FORM_TOTAL_WIDGET;

	    /* si on se retrouve sur la date et que etat.entree = 0,
	       on enregistre l'op�ration */

	    if ( no_widget == SCHEDULER_FORM_DATE &&
		 etat.entree == 0 )
	    {
		fin_edition_echeance ();
		return TRUE;
	    }

	    /* si le prochain est le d�bit, on v�rifie s'il n'y a rien dans
	       cette entr�e et s'il y a quelque chose dans l'entr�e du
	       cr�dit */

	    if ( no_widget == SCHEDULER_FORM_DEBIT )
	    {
		/* si le d�bit est gris et le cr�dit est noir, on met sur le cr�dit */

		if ( gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ) == style_entree_formulaire[ENGRIS] &&
		     gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ) == style_entree_formulaire[ENCLAIR] )
		    no_widget = SCHEDULER_FORM_CREDIT;
	    }

	    /* si le prochain est le cr�dit et que le d�bit a �t� rempli,
	       on se met sur la devise et on efface le cr�dit */

	    if ( no_widget == SCHEDULER_FORM_CREDIT )
		if ( gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ) == style_entree_formulaire[ENCLAIR] )
		{
		    no_widget = SCHEDULER_FORM_DEVISE;
		    gtk_widget_set_style (widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT],
					  style_entree_formulaire[ENGRIS] );
		    gtk_entry_set_text ( GTK_ENTRY (widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT]),
					 _("Credit") );
		}

	    /* on s�lectionne le contenu de la nouvelle entr�e */
	    if ( GTK_IS_COMBOFIX ( widget_formulaire_echeancier[no_widget] ) )
	    {
		gtk_widget_grab_focus ( GTK_COMBOFIX ( widget_formulaire_echeancier[no_widget] ) -> entry );
		gtk_entry_select_region ( GTK_ENTRY ( GTK_COMBOFIX ( widget_formulaire_echeancier[no_widget] ) -> entry ), 0, -1 );
	    }
	    else
	    {
		if ( GTK_IS_ENTRY ( widget_formulaire_echeancier[no_widget] ) )
		    gtk_entry_select_region ( GTK_ENTRY ( widget_formulaire_echeancier[no_widget] ), 0, -1 );
		gtk_widget_grab_focus ( widget_formulaire_echeancier[no_widget]  );
	    }
	    return TRUE;

	case GDK_Return :		/* touches entr�e */
	case GDK_KP_Enter :

	    if ( !etat.formulaire_echeance_dans_fenetre )
	    {
		/*	 gtk_signal_emit_stop_by_name ( GTK_OBJECT ( widget ),
			 "key-press-event");
			 */
		/* si la touche CTRL est elle aussi active, alors c'est que l'on est
		   probablement sur un champ de date et que l'on souhaite ouvrir
		   un calendrier */
		if ( ( ev -> state & GDK_CONTROL_MASK ) == GDK_CONTROL_MASK )
		{
		    switch ( no_widget )
		    {
			case SCHEDULER_FORM_DATE :
			case SCHEDULER_FORM_FINAL_DATE :

			    popup_cal = gsb_calendar_new ( widget );
			    gtk_signal_connect_object ( GTK_OBJECT ( popup_cal ),
							"destroy",
							GTK_SIGNAL_FUNC ( ferme_calendrier ),
							GTK_OBJECT ( widget ) );
			    gtk_widget_grab_focus ( GTK_WIDGET ( popup_cal ) );
			    break ;

			default :
			    break ;
		    }
		}
		/* si la touche CTRL n'est pas active, alors on valide simplement
		   la saisie de l'�ch�ance */
		else
		{
		    /* on fait perdre le focus au widget courant pour faire
		       les changements automatiques si n�cessaire */

		    gtk_widget_grab_focus ( liste_echeances );
		    fin_edition_echeance ();
		}
	    }
	    return FALSE;
	    break;

	case GDK_plus :		/* touches + */
	case GDK_KP_Add :

	    /* si on est dans une entree de date, on augmente d'un jour
	       (d'une semaine) la date */

	    if ( no_widget == SCHEDULER_FORM_DATE ||
		 no_widget == SCHEDULER_FORM_FINAL_DATE )
	    {
		gtk_signal_emit_stop_by_name ( GTK_OBJECT ( widget ),
					       "key-press-event");
		if ( ( ev -> state & GDK_SHIFT_MASK ) != GDK_SHIFT_MASK )
		    inc_dec_date ( widget_formulaire_echeancier[no_widget], ONE_DAY );
		else
		    inc_dec_date ( widget_formulaire_echeancier[no_widget], ONE_WEEK );
		return TRUE;
	    }
	    return FALSE;

	case GDK_minus :		/* touches - */
	case GDK_KP_Subtract :

	    /* si on est dans une entree de date, on diminue d'un jour
	       (d'une semaine) la date */

	    if ( no_widget == SCHEDULER_FORM_DATE ||
		 no_widget == SCHEDULER_FORM_FINAL_DATE )
	    {
		gtk_signal_emit_stop_by_name ( GTK_OBJECT ( widget ),
					       "key-press-event");
		if ( ( ev -> state & GDK_SHIFT_MASK ) != GDK_SHIFT_MASK )
		    inc_dec_date ( widget_formulaire_echeancier[no_widget], - ONE_DAY );
		else
		    inc_dec_date ( widget_formulaire_echeancier[no_widget], - ONE_WEEK );
		return TRUE;
	    }
	    return FALSE;

	case GDK_Page_Up :		/* touche PgUp */
	case GDK_KP_Page_Up :

	    /* si on est dans une entree de date,
	       on augmente d'un mois (d'un an) la date */

	    if ( no_widget == SCHEDULER_FORM_DATE ||
		 no_widget == SCHEDULER_FORM_FINAL_DATE )
	    {
		gtk_signal_emit_stop_by_name ( GTK_OBJECT ( widget ),
					       "key-press-event");
		if ( ( ev -> state & GDK_SHIFT_MASK ) != GDK_SHIFT_MASK )
		    inc_dec_date ( widget_formulaire_echeancier[no_widget], ONE_MONTH );
		else
		    inc_dec_date ( widget_formulaire_echeancier[no_widget], ONE_YEAR );
		return TRUE;
	    }
	    return FALSE;

	case GDK_Page_Down :		/* touche PgDown */
	case GDK_KP_Page_Down :

	    /* si on est dans une entree de date,
	       on augmente d'un mois (d'un an) la date */

	    if ( no_widget == SCHEDULER_FORM_DATE ||
		 no_widget == SCHEDULER_FORM_FINAL_DATE )
	    {
		gtk_signal_emit_stop_by_name ( GTK_OBJECT ( widget ),
					       "key-press-event");
		if ( ( ev -> state & GDK_SHIFT_MASK ) != GDK_SHIFT_MASK )
		    inc_dec_date ( widget_formulaire_echeancier[no_widget], - ONE_MONTH );
		else
		    inc_dec_date ( widget_formulaire_echeancier[no_widget], - ONE_YEAR );
		return TRUE;
	    }
	    return FALSE;

	default:
	    /* Reverting to default handler */
	    return FALSE;
    }
    return TRUE;
}
/******************************************************************************/

/******************************************************************************/
void affiche_date_limite_echeancier ( void )
{
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] );
}
/******************************************************************************/

/******************************************************************************/
void cache_date_limite_echeancier ( void )
{
    gtk_widget_hide ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] );
}
/******************************************************************************/

/******************************************************************************/
void affiche_personnalisation_echeancier ( void )
{
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_NB] );
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU] );
}
/******************************************************************************/

/******************************************************************************/
void cache_personnalisation_echeancier ( void )
{
    gtk_widget_hide ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_NB] );
    gtk_widget_hide ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU] );
}
/******************************************************************************/

/******************************************************************************/
/* Fonction fin_edition_echeance */
/* appel�e pour soit valider une modif d'�ch�ance ( ou nouvelle ), */
/* ou pour saisir l'�ch�ance */
/******************************************************************************/
void fin_edition_echeance ( void )
{
    struct operation_echeance *echeance;
    gchar **tableau_char;
    gchar *pointeur_char;
    GSList *pointeur_liste;
    gint rafraichir_categ;
    gint compte_virement;

    compte_virement = 0;
    rafraichir_categ = 0;

    /* on v�rifie que les date et date_limite sont correctes */

    if ( !modifie_date ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE] ))
    {
	dialogue_error ( PRESPACIFY(_("Invalid date")) );
	gtk_widget_grab_focus ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE] );
	gtk_entry_select_region ( GTK_ENTRY (  widget_formulaire_echeancier[SCHEDULER_FORM_DATE]),
				  0,
				  -1);
	return;
    }

    if ( gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] ) == style_entree_formulaire[ENCLAIR]
	 &&
	 strcmp ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] ))),
		  _("None") ))
	if ( !modifie_date ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] ))
	{
	    dialogue_error ( PRESPACIFY(_("Invalid limit date")) );
	    gtk_widget_grab_focus ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] );
	    gtk_entry_select_region ( GTK_ENTRY (  widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE]),
				      0,
				      -1);
	    return;
	}

    /* v�rification que ce n'est pas un virement sur lui-m�me */

    if ( !g_strcasecmp ( g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ))),
			 g_strconcat ( COLON(_("Transfer")),
				       COMPTE_ECHEANCE,
				       NULL )))
    {
	dialogue_error ( _("Can't issue a transfer its own account.") );
	return;
    }

    /* si c'est un virement, on v�rifie que le compte existe  */


    pointeur_char = g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] )));

    if ( !g_strncasecmp ( pointeur_char, _("Transfer"), 8 ))
    {
	gint i;

	tableau_char = g_strsplit ( pointeur_char, ":", 2 );

	/* S'il n'y a rien apr�s "Transfer", alors : */
	if ( !tableau_char[1] ||
	     !strlen ( tableau_char[1] ) )
	{
	    dialogue_error ( _("There is no associated account for this transfer.") );
	    return;
	}

	if ( tableau_char[1] )
	{
	    tableau_char[1] = g_strstrip ( tableau_char[1] );

	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

	    compte_virement = -1;

	    for ( i = 0 ; i < nb_comptes ; i++ )
	    {
		if ( !g_strcasecmp ( NOM_DU_COMPTE,
				     tableau_char[1] ) )
		    compte_virement = i;
		p_tab_nom_de_compte_variable++;
	    }

	    if ( compte_virement == -1 )
	    {
		dialogue_warning ( _("Associated account of this transfer is invalid.") );
		return;
	    }
	}
	g_strfreev ( tableau_char );
    }

    /* v�rification si c'est une �ch�ance auto que ce n'est pas un ch�que */

    if ( GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE]  ) -> menu_item ),
						 "auto_man" )) == 1
	 &&
	 GTK_WIDGET_VISIBLE ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] )
	 &&
	 ((struct struct_type_ope  *)( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) -> menu_item ),
							     "adr_type" )))->numerotation_auto )
    {
	dialogue ( PRESPACIFY(_("Impossible to create or entry an automatic scheduled transaction\n with a cheque or a method of payment with an automatic incremental number.")) );
	return;
    }

    /* r�cup�ration de l'op�ration : soit l'adr de la struct, soit NULL si nouvelle */

    echeance = gtk_object_get_data ( GTK_OBJECT ( formulaire_echeancier ),
				     "adr_echeance" );

    /* on s�pare ici en 2 parties : si le label label_saisie_modif contient Modification, c'est une modif ou une nvelle �ch�ance, */
    /* s'il contient Saisie, on enregistre l'op� */

    if ( strcmp ( GTK_LABEL ( label_saisie_modif ) -> label,
		  _("Input") ) )
    {
	/*       on commence ici la partie modification / nouvelle �ch�ance */

	/* si c'est une nouvelle �ch�ance, on la cr�e */

	if ( !echeance )
	    echeance = calloc ( 1,
				sizeof ( struct operation_echeance ));

	/* r�cup�re la date */


	pointeur_char = g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY (widget_formulaire_echeancier[SCHEDULER_FORM_DATE] )));

	tableau_char = g_strsplit ( pointeur_char,
				    "/",
				    3 );


	echeance -> jour = my_strtod ( tableau_char[0],
				       NULL );
	echeance -> mois = my_strtod ( tableau_char[1],
				       NULL );
	echeance -> annee = my_strtod (tableau_char[2],
				       NULL );

	echeance ->date = g_date_new_dmy ( echeance ->jour,
					   echeance ->mois,
					   echeance ->annee);


	/* r�cup�ration du tiers, s'il n'existe pas, on le cr�e */

	if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] ) -> entry ) == style_entree_formulaire[ENCLAIR] )
	{
	    pointeur_char = g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] )));

	    if ( ( pointeur_liste = g_slist_find_custom ( liste_struct_tiers,
							  pointeur_char,
							  ( GCompareFunc ) recherche_tiers_par_nom )) )
		echeance -> tiers = (( struct struct_tiers * )( pointeur_liste -> data )) -> no_tiers;
	    else
	    {
		echeance -> tiers = (( struct struct_tiers * )( ajoute_nouveau_tiers ( pointeur_char ))) -> no_tiers;
		mise_a_jour_tiers();
	    }
	}



	/* r�cup�ration du montant */

	if ( gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ) == style_entree_formulaire[ENCLAIR] )
	    echeance -> montant = -my_strtod ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ))),
					       NULL );
	else
	    echeance -> montant = my_strtod ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ))),
					      NULL );

	/* r�cup�ration de la devise */

	echeance -> devise = ((struct struct_devise *)( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE] ) -> menu_item ),
									      "adr_devise" ))) -> no_devise;

	/* r�cup�ration du no de compte */

	echeance -> compte = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT]  ) -> menu_item ),
								     "no_compte" ) );


	/*   r�cup�ration des cat�gories / sous-cat�g, s'ils n'existent pas, on les cr�e */

	if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ) -> entry ) == style_entree_formulaire[ENCLAIR] )
	{
	    struct struct_categ *categ;

	    tableau_char = g_strsplit ( g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ))),
					":",
					2 );

	    tableau_char[0] = g_strstrip ( tableau_char[0] );

	    if ( tableau_char[1] )
		tableau_char[1] = g_strstrip ( tableau_char[1] );


	    if ( strlen ( tableau_char[0] ) )
	    {
		if ( !strcmp ( tableau_char[0],
			       _("Transfer") )
		     && tableau_char[1]
		     && strlen ( tableau_char[1]) )
		{
		    /* c'est un virement, il n'y a donc aucune cat�gorie */

		    gint i;
		    echeance -> categorie = 0;
		    echeance -> sous_categorie = 0;

		    /* recherche le no de compte du virement */

		    p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

		    for ( i = 0 ; i < nb_comptes ; i++ )
		    {
			if ( !g_strcasecmp ( NOM_DU_COMPTE,
					     tableau_char[1] ) )
			    echeance -> compte_virement = i;

			p_tab_nom_de_compte_variable++;
		    }
		}
		else
		{
		    pointeur_liste = g_slist_find_custom ( liste_struct_categories,
							   tableau_char[0],
							   ( GCompareFunc ) recherche_categorie_par_nom );

		    if ( pointeur_liste )
		    {
			categ = pointeur_liste -> data;
			rafraichir_categ = 0;
		    }
		    else
		    {
			categ = ajoute_nouvelle_categorie ( tableau_char[0] );
			rafraichir_categ = 1;
		    }

		    echeance -> categorie = categ -> no_categ;

		    if ( tableau_char[1] && strlen (tableau_char[1]) )
		    {
			struct struct_sous_categ *sous_categ;

			pointeur_liste = g_slist_find_custom ( categ -> liste_sous_categ,
							       tableau_char[1],
							       ( GCompareFunc ) recherche_sous_categorie_par_nom );

			if ( pointeur_liste )
			{
			    sous_categ = pointeur_liste -> data;
			    rafraichir_categ = 0;
			}
			else
			{
			    sous_categ = ajoute_nouvelle_sous_categorie ( tableau_char[1],
									  categ );
			    rafraichir_categ = 1;
			}

			echeance -> sous_categorie = sous_categ -> no_sous_categ;
		    }

		    echeance -> compte_virement = 0;
		}
	    }
	    g_strfreev ( tableau_char );
	}
	else
	    echeance -> compte_virement = -1;

	/* r�cup�ration du type d'op�ration */

	if ( GTK_WIDGET_VISIBLE ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ))
	{
	    echeance -> type_ope = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) -> menu_item ),
									   "no_type" ));

	    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] )
		 &&
		 gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] ) == style_entree_formulaire[ENCLAIR] )
		echeance -> contenu_type = g_strdup ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] ))));
	}

	/* r�cup�ration du no d'exercice */

	echeance -> no_exercice = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] ) -> menu_item ),
									  "no_exercice" ));

	/* r�cup�ration de l'imputation budg�taire */

	if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ) -> entry ) == style_entree_formulaire[ENCLAIR] )
	{
	    struct struct_imputation *imputation;

	    pointeur_char = gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ));

	    tableau_char = g_strsplit ( pointeur_char,
					":",
					2 );

	    tableau_char[0] = g_strstrip ( tableau_char[0] );

	    if ( tableau_char[1] )
		tableau_char[1] = g_strstrip ( tableau_char[1] );

	    pointeur_liste = g_slist_find_custom ( liste_struct_imputation,
						   tableau_char[0],
						   ( GCompareFunc ) recherche_imputation_par_nom );

	    if ( pointeur_liste )
		imputation = pointeur_liste -> data;
	    else
	    {
		imputation = ajoute_nouvelle_imputation ( tableau_char[0] );

		if ( echeance -> montant < 0 )
		    imputation -> type_imputation = 1;
		else
		    imputation -> type_imputation = 0;
	    }

	    echeance -> imputation = imputation -> no_imputation;

	    if ( tableau_char[1] && strlen (tableau_char[1]) )
	    {
		struct struct_sous_imputation *sous_imputation;

		pointeur_liste = g_slist_find_custom ( imputation -> liste_sous_imputation,
						       tableau_char[1],
						       ( GCompareFunc ) recherche_sous_imputation_par_nom );

		if ( pointeur_liste )
		    sous_imputation = pointeur_liste -> data;
		else
		    sous_imputation = ajoute_nouvelle_sous_imputation ( tableau_char[1],
									imputation );

		echeance -> sous_imputation = sous_imputation -> no_sous_imputation;
	    }
	    else
		echeance -> sous_imputation = 0;

	    g_strfreev ( tableau_char );
	}

	/*       r�cup�ration de auto_man */

	echeance -> auto_man = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE]  ) -> menu_item ),
								       "auto_man" ) );

	/* r�cup�ration des notes */

	if ( gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] ) == style_entree_formulaire[ENCLAIR] )
	    echeance -> notes = g_strdup ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] ))));


	/* r�cup�ration de la fr�quence */

	echeance -> periodicite = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY]  ) -> menu_item ),
									  "periodicite" ) );

	if ( echeance -> periodicite == 4 )
	{
	    echeance -> intervalle_periodicite_personnalisee = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU]  ) -> menu_item ),
												       "intervalle_perso" ) );

	    echeance -> periodicite_personnalisee = my_strtod ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_NB] )),
								NULL );
	}

	if ( echeance -> periodicite
	     &&
	     gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] ) == style_entree_formulaire[ENCLAIR] )
	{
	    /* traitement de la date limite */

	    tableau_char = g_strsplit ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] ))),
					"/",
					3 );

	    echeance -> jour_limite = my_strtod ( tableau_char[0],
						  NULL );
	    echeance -> mois_limite = my_strtod ( tableau_char[1],
						  NULL );
	    echeance -> annee_limite = my_strtod (tableau_char[2],
						  NULL );

	    echeance->date_limite = g_date_new_dmy ( echeance->jour_limite,
						     echeance->mois_limite,
						     echeance->annee_limite);
	}


	/* si c'est une nouvelle op�, on lui met un no et on l'ajoute � la liste */

	if ( !echeance -> no_operation )
	{
	    echeance -> no_operation = ++no_derniere_echeance;
	    nb_echeances++;
	    gsliste_echeances = g_slist_insert_sorted ( gsliste_echeances,
							echeance,
							(GCompareFunc) comparaison_date_echeance );
	}
    }
    else
    {
	/* on commence ici la partie saisie de l'�ch�ance */

	struct structure_operation *operation;
	gint virement;
	struct struct_devise *devise;

	/* cr�e l'op�ration */

	operation = calloc ( 1,
			     sizeof ( struct structure_operation ) ); 


	/* r�cup�re la date */


	pointeur_char = g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY (widget_formulaire_echeancier[SCHEDULER_FORM_DATE] )));

	tableau_char = g_strsplit ( pointeur_char,
				    "/",
				    3 );


	operation -> jour = my_strtod ( tableau_char[0],
					NULL );
	operation -> mois = my_strtod ( tableau_char[1],
					NULL );
	operation -> annee = my_strtod (tableau_char[2],
					NULL );

	operation ->date = g_date_new_dmy ( operation ->jour,
					    operation ->mois,
					    operation ->annee);


	/* r�cup�ration du no de compte */

	operation -> no_compte = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT]  ) -> menu_item ),
									 "no_compte" ) );


	/* r�cup�ration du tiers, s'il n'existe pas, on le cr�e */

	if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] ) -> entry ) == style_entree_formulaire[ENCLAIR] )
	{
	    pointeur_char = g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] )));

	    if ( ( pointeur_liste = g_slist_find_custom ( liste_struct_tiers,
							  pointeur_char,
							  ( GCompareFunc ) recherche_tiers_par_nom )) )
		operation -> tiers = (( struct struct_tiers * )( pointeur_liste -> data )) -> no_tiers;
	    else
	    {
		operation -> tiers = (( struct struct_tiers * )( ajoute_nouveau_tiers ( pointeur_char ))) -> no_tiers;
		mise_a_jour_tiers ();
	    }
	}


	/* r�cup�ration du montant */

	if ( gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ) == style_entree_formulaire[ENCLAIR] )
	    operation -> montant = -my_strtod ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ))),
						NULL );
	else
	    operation -> montant = my_strtod ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ))),
					       NULL );

	/* r�cup�ration de la devise */

	devise = gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE] ) -> menu_item),
				       "adr_devise" );


	/* si c'est la devise du compte ou si c'est un compte qui doit passer � l'euro ( la transfo se fait au niveau de l'affichage de la liste ) */
	/* ou si c'est un compte en euro et l'op� est dans une devise qui doit passer � l'euro -> ok */

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

	if ( !devise_compte
	     ||
	     devise_compte -> no_devise != DEVISE )
	    devise_compte = g_slist_find_custom ( liste_struct_devises,
						  GINT_TO_POINTER ( DEVISE ),
						  ( GCompareFunc ) recherche_devise_par_no ) -> data;

	operation -> devise = devise -> no_devise;

	if ( !( operation -> no_operation
		||
		devise -> no_devise == DEVISE
		||
		( devise_compte -> passage_euro && !strcmp ( devise -> nom_devise, _("Euro") ))
		||
		( !strcmp ( devise_compte -> nom_devise, _("Euro") ) && devise -> passage_euro )))
	{
	    /* c'est une devise �trang�re, on demande le taux de change et les frais de change */

	    demande_taux_de_change ( devise_compte, devise, 1,
				     (gdouble ) 0, (gdouble ) 0, FALSE );

	    operation -> taux_change = taux_de_change[0];
	    operation -> frais_change = taux_de_change[1];

	    if ( operation -> taux_change < 0 )
	    {
		operation -> taux_change = -operation -> taux_change;
		operation -> une_devise_compte_egale_x_devise_ope = 1;
	    }
	}


	/*   r�cup�ration des cat�gories / sous-cat�g, s'ils n'existent pas, on les cr�e */

	virement = 0;

	if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ) -> entry ) == style_entree_formulaire[ENCLAIR] )
	{
	    struct struct_categ *categ;

	    tableau_char = g_strsplit ( g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ))),
					":",
					2 );

	    tableau_char[0] = g_strstrip ( tableau_char[0] );

	    if ( tableau_char[1] )
		tableau_char[1] = g_strstrip ( tableau_char[1] );


	    if ( strlen ( tableau_char[0] ) )
	    {
		if ( !strcmp ( tableau_char[0],
			       _("Transfer") )
		     && tableau_char[1]
		     && strlen ( tableau_char[1]) )
		{
		    /* c'est un virement, il n'y a donc aucune cat�torie */

		    operation -> categorie = 0;
		    operation -> sous_categorie = 0;
		    virement = 1;
		}
		else
		{
		    pointeur_liste = g_slist_find_custom ( liste_struct_categories,
							   tableau_char[0],
							   ( GCompareFunc ) recherche_categorie_par_nom );

		    if ( pointeur_liste )
		    {
			categ = pointeur_liste -> data;
			rafraichir_categ = 0;
		    }
		    else
		    {
			categ = ajoute_nouvelle_categorie ( tableau_char[0] );
			rafraichir_categ = 1;
		    }

		    operation -> categorie = categ -> no_categ;

		    if ( tableau_char[1] && strlen (tableau_char[1]) )
		    {
			struct struct_sous_categ *sous_categ;

			pointeur_liste = g_slist_find_custom ( categ -> liste_sous_categ,
							       tableau_char[1],
							       ( GCompareFunc ) recherche_sous_categorie_par_nom );

			if ( pointeur_liste )
			{
			    sous_categ = pointeur_liste -> data;
			    rafraichir_categ = 0;
			}
			else
			{
			    sous_categ = ajoute_nouvelle_sous_categorie ( tableau_char[1],
									  categ );
			    rafraichir_categ = 1;
			}

			operation -> sous_categorie = sous_categ -> no_sous_categ;
		    }
		}
	    }
	    g_strfreev ( tableau_char );
	}

	/* r�cup�ration du type d'op�ration */

	if ( GTK_WIDGET_VISIBLE ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ))
	{
	    operation -> type_ope = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) -> menu_item ),
									    "no_type" ));

	    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] )
		 &&
		 gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] ) == style_entree_formulaire[ENCLAIR] )
	    {
		struct struct_type_ope *type;

		type = gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) -> menu_item ),
					     "adr_type" );

		operation -> contenu_type = g_strdup ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] ))));

		if ( type -> numerotation_auto )
		    type -> no_en_cours = ( atoi ( operation -> contenu_type ));
	    }
	}


	/* r�cup�ration du no d'exercice */

	if ( GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] ) -> menu_item ),
						     "no_exercice" )) == -2 )
	    operation -> no_exercice = recherche_exo_correspondant ( operation -> date );
	else
	    operation -> no_exercice = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] ) -> menu_item ),
									       "no_exercice" ));


	/* r�cup�ration de l'imputation budg�taire */

	if ( gtk_widget_get_style ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ) -> entry ) == style_entree_formulaire[ENCLAIR] )
	{
	    struct struct_imputation *imputation;

	    pointeur_char = gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ));

	    tableau_char = g_strsplit ( pointeur_char,
					":",
					2 );

	    tableau_char[0] = g_strstrip ( tableau_char[0] );

	    if ( tableau_char[1] )
		tableau_char[1] = g_strstrip ( tableau_char[1] );

	    pointeur_liste = g_slist_find_custom ( liste_struct_imputation,
						   tableau_char[0],
						   ( GCompareFunc ) recherche_imputation_par_nom );

	    if ( pointeur_liste )
		imputation = pointeur_liste -> data;
	    else
	    {
		imputation = ajoute_nouvelle_imputation ( tableau_char[0] );

		if ( operation -> montant < 0 )
		    imputation -> type_imputation = 1;
		else
		    imputation -> type_imputation = 0;
	    }

	    operation -> imputation = imputation -> no_imputation;

	    if ( tableau_char[1] && strlen (tableau_char[1]) )
	    {
		struct struct_sous_imputation *sous_imputation;

		pointeur_liste = g_slist_find_custom ( imputation -> liste_sous_imputation,
						       tableau_char[1],
						       ( GCompareFunc ) recherche_sous_imputation_par_nom );

		if ( pointeur_liste )
		    sous_imputation = pointeur_liste -> data;
		else
		    sous_imputation = ajoute_nouvelle_sous_imputation ( tableau_char[1],
									imputation );

		operation -> sous_imputation = sous_imputation -> no_sous_imputation;
	    }
	    else
		operation -> sous_imputation = 0;

	    g_strfreev ( tableau_char );
	}

	/*       r�cup�ration de auto_man */

	operation -> auto_man = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE]  ) -> menu_item ),
									"auto_man" ) );

	/* r�cup�ration des notes */

	if ( gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] ) == style_entree_formulaire[ENCLAIR] )
	    operation -> notes = g_strdup ( g_strstrip ( (gchar *) gtk_entry_get_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] ))));


	/*   on a fini de remplir l'op�, on peut l'ajouter � la liste */

	ajout_operation ( operation );



	/*   si c'�tait un virement, on cr�e une copie de l'op�, on l'ajout � la liste puis on remplit les relations */

	if ( virement )
	{
	    struct structure_operation *operation_2;
	    struct struct_devise *devise_compte_2;

	    operation_2 = calloc ( 1,
				   sizeof ( struct structure_operation ) );

	    operation_2 -> no_compte = compte_virement;


	    /* remplit la nouvelle op� */

	    operation_2 -> jour = operation -> jour;
	    operation_2 -> mois = operation -> mois;
	    operation_2 -> annee = operation -> annee;
	    operation_2 ->date = g_date_new_dmy ( operation_2->jour,
						  operation_2->mois,
						  operation_2->annee);
	    operation_2 -> montant = -operation -> montant;

	    /* si c'est la devise du compte ou si c'est un compte qui doit passer � l'euro ( la transfo se fait au niveau */
	    /* de l'affichage de la liste ) ou si c'est un compte en euro et l'op� est dans une devise qui doit passer � l'euro -> ok */

	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + compte_virement;

	    devise_compte_2 = g_slist_find_custom ( liste_struct_devises,
						    GINT_TO_POINTER ( DEVISE ),
						    ( GCompareFunc ) recherche_devise_par_no ) -> data;

	    operation_2 -> devise = operation -> devise;

	    if ( !( operation_2-> no_operation
		    ||
		    devise -> no_devise == DEVISE
		    ||
		    ( devise_compte_2 -> passage_euro && !strcmp ( devise -> nom_devise, _("Euro") ))
		    ||
		    ( !strcmp ( devise_compte_2 -> nom_devise, _("Euro") ) && devise -> passage_euro )))
	    {
		/* c'est une devise �trang�re, on demande le taux de change et les frais de change */

		demande_taux_de_change ( devise_compte_2, devise, 1,
					 (gdouble ) 0, (gdouble ) 0, FALSE );

		operation_2 -> taux_change = taux_de_change[0];
		operation_2 -> frais_change = taux_de_change[1];

		if ( operation_2 -> taux_change < 0 )
		{
		    operation_2 -> taux_change = -operation_2 -> taux_change;
		    operation_2 -> une_devise_compte_egale_x_devise_ope = 1;
		}
	    }
	    else
	    {
		operation_2 -> taux_change = 0;
		operation_2 -> frais_change = 0;
	    }



	    operation_2 -> tiers = operation -> tiers;
	    operation_2 -> categorie = 0;
	    operation_2 -> sous_categorie = 0;

	    if ( operation -> notes )
		operation_2 -> notes = g_strdup ( operation -> notes);

	    operation_2 -> auto_man = operation -> auto_man;
	    operation_2 -> type_ope = operation -> type_ope;

	    if ( operation -> contenu_type )
		operation_2 -> contenu_type = g_strdup ( operation -> contenu_type );

	    operation_2 -> no_exercice = operation -> no_exercice;
	    operation_2 -> imputation = operation -> imputation;
	    operation_2 -> sous_imputation = operation -> sous_imputation;

	    /*   on a fini de remplir l'op�, on peut l'ajouter � la liste */

	    ajout_operation ( operation_2 );


	    /* on met maintenant les relations entre les diff�rentes op� */

	    operation -> relation_no_operation = operation_2 -> no_operation;
	    operation -> relation_no_compte = operation_2 -> no_compte;
	    operation_2 -> relation_no_operation = operation -> no_operation;
	    operation_2 -> relation_no_compte = operation -> no_compte;

	    /* on met � jour le compte courant pour le virement (il a �t� mis � jour avec ajout op�ration, mais sans les liens de virement) */

	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

	    MISE_A_JOUR = 1;
	    verification_mise_a_jour_liste ();
	}


	/* passe l'�ch�ance � la prochaine date */

	incrementation_echeance ( echeance );

	/* r�affiche les option menu de la p�riodicit�, des banques et de l'automatisme, effac�s pour une saisie d'op� */

	gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE] );
	gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY] );
	gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY] );

    }


    if ( rafraichir_categ )
	mise_a_jour_categ ();


    formulaire_echeancier_a_zero ();

    if ( !etat.formulaire_echeancier_toujours_affiche )
	gtk_widget_hide ( frame_formulaire_echeancier );

    update_liste_echeances_manuelles_accueil ();
    update_liste_echeances_auto_accueil ();
    remplissage_liste_echeance ();
    gtk_widget_grab_focus ( liste_echeances );

    modification_fichier ( TRUE );
}
/******************************************************************************/

/******************************************************************************/
/* cette proc�dure compare 2 struct d'�ch�ances entre elles au niveau de la date */
/* pour le classement */
/******************************************************************************/
gint comparaison_date_echeance (  struct operation_echeance *echeance_1,
				  struct operation_echeance *echeance_2)
{
    return ( g_date_compare ( echeance_1 -> date,
			      echeance_2 -> date ));
}
/******************************************************************************/

/******************************************************************************/
void formulaire_echeancier_a_zero ( void )
{
    /* on met les styles des entr�es au gris */

    gtk_widget_set_style ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] ) -> entry,
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ) -> entry,
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ) -> entry,
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_echeancier[SCHEDULER_FORM_BANK],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_echeancier[SCHEDULER_FORM_VOUCHER],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES],
			   style_entree_formulaire[ENGRIS] );
    gtk_widget_set_style ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE],
			   style_entree_formulaire[ENGRIS] );

    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_DATE] ),
			 _("Date") );
    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY] ),
			    _("Third party") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ),
			 _("Debit") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ),
			 _("Credit") );
    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ),
			    _("Categories : Sub-categories") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] ),
			 _("Transfer reference") );
    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ),
			    _("Budgetary line") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_BANK] ),
			 _("Bank references") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_VOUCHER] ),
			 _("Voucher") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] ),
			 _("Notes") );
    gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] ),
			 _("Limit date") );

    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE] ),
				  0 );
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] ),
				  0 );
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE] ),
				  0 );
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY] ),
				  0 );
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU] ),
				  0 );
    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ),
				  0 );
    changement_choix_compte_echeancier ();


    gtk_widget_hide ( widget_formulaire_echeancier[SCHEDULER_FORM_FINAL_DATE] );
    gtk_widget_hide ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_NB] );
    gtk_widget_hide ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU] );


    gtk_widget_set_sensitive ( widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE],
			       FALSE );
    gtk_widget_set_sensitive ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT],
			       FALSE );
    gtk_widget_set_sensitive ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE],
			       FALSE );
    gtk_widget_set_sensitive ( widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE],
			       FALSE );
    gtk_widget_set_sensitive ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE],
			       FALSE );
    gtk_widget_set_sensitive ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY],
			       FALSE );
    gtk_widget_set_sensitive ( hbox_valider_annuler_echeance,
			       FALSE );

    gtk_widget_hide ( label_saisie_modif );
    gtk_label_set_text ( GTK_LABEL ( label_saisie_modif ),
			 _("Modification") );

    /* r�affiche les boutons effac�s pour une saisie */

    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] );
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE] );
    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY] );

    /* on associe au formulaire l'adr de l'�ch�ance courante */

    gtk_object_set_data ( GTK_OBJECT ( formulaire_echeancier ),
			  "adr_echeance",
			  NULL );
}
/******************************************************************************/

/******************************************************************************/
void incrementation_echeance ( struct operation_echeance *echeance )
{
    GDate *date_suivante;

    /* p�riodicit� hebdomadaire */
    if ( echeance -> periodicite == 1 )
    {
	g_date_add_days ( echeance -> date,
			  7 );

	/* magouille car il semble y avoir un bug dans g_date_add_days qui ne fait pas l'addition si on ne met pas la ligne suivante */
	g_date_add_months ( echeance -> date,
			    0 );
    }
    else
	/* p�riodicit� mensuelle */
	if ( echeance -> periodicite == 2 )
	    g_date_add_months ( echeance -> date,
				1 );
	else
	    /* p�riodicit� annuelle */
	    if ( echeance -> periodicite == 3 )
		g_date_add_years ( echeance -> date,
				   1 );
	    else
		/* p�riodicit� perso */
		if ( !echeance -> intervalle_periodicite_personnalisee )
		{
		    g_date_add_days ( echeance -> date,
				      echeance -> periodicite_personnalisee );

		    /* magouille car il semble y avoir un bug dans g_date_add_days qui ne fait pas l'addition si on ne met pas la ligne suivante */
		    g_date_add_months ( echeance -> date,
					0 );
		}
		else
		    if ( echeance -> intervalle_periodicite_personnalisee == 1 )
			g_date_add_months ( echeance -> date,
					    echeance -> periodicite_personnalisee );
		    else
			g_date_add_years ( echeance -> date,
					   echeance -> periodicite_personnalisee );

    /* on recommence l'incr�mentation sur la copie de la date pour avoir la date suivante */
    /* permet de voir si c'�tait la derni�re incr�mentation */

    date_suivante = g_date_new_dmy ( echeance -> date -> day,
				     echeance -> date -> month,
				     echeance -> date -> year ); 


    /* p�riodicit� hebdomadaire */

    if ( echeance -> periodicite == 1 )
    {
	g_date_add_days ( date_suivante,
			  7 );
	/* magouille car il semble y avoir un bug dans g_date_add_days qui ne fait pas l'addition si on ne met pas la ligne suivante */
	g_date_add_months ( echeance -> date,
			    0 );
    }
    else
	/* p�riodicit� mensuelle */
	if ( echeance -> periodicite == 2 )
	    g_date_add_months ( date_suivante,
				1 );
	else
	    /* p�riodicit� annuelle */
	    if ( echeance -> periodicite == 3 )
		g_date_add_years ( date_suivante,
				   1 );
	    else
		/* p�riodicit� perso */
		if ( !echeance -> intervalle_periodicite_personnalisee )
		{
		    /* magouille car il semble y avoir un bug dans g_date_add_days qui ne fait pas l'addition si on ne met pas la ligne suivante */
		    g_date_add_months ( echeance -> date,
					0 );
		    g_date_add_days ( date_suivante,
				      echeance -> periodicite_personnalisee );
		}
		else
		    if ( echeance -> intervalle_periodicite_personnalisee == 1 )
			g_date_add_months ( date_suivante,
					    echeance -> periodicite_personnalisee );
		    else
			g_date_add_years ( date_suivante,
					   echeance -> periodicite_personnalisee );


    /* si l'�ch�ance est finie, on la vire, sinon on met � jour les var jour, mois et ann�e */

    if ( !echeance -> periodicite 
	 ||
	 (
	  echeance -> date_limite
	  &&
	  g_date_compare ( echeance -> date,
			   echeance -> date_limite ) > 0  ))
    {
	GtkWidget *label;

	if ( GTK_BIN ( frame_etat_echeances_finies ) -> child )
	{
	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + echeance ->compte;

	    if ( echeance -> montant >= 0 )
		label = gtk_label_new ( g_strdup_printf (PRESPACIFY(_("%4.2f %s credit on %s")),
							 echeance ->montant,
							 devise_name ((struct struct_devise *)(g_slist_find_custom ( liste_struct_devises,
														     GINT_TO_POINTER ( echeance -> devise ),
														     (GCompareFunc) recherche_devise_par_no )->data)),
							 NOM_DU_COMPTE ));
	    else
		label = gtk_label_new ( g_strdup_printf (PRESPACIFY(_("%4.2f %s debit on %s")),
							 -echeance ->montant,
							 devise_name ((struct struct_devise *)(g_slist_find_custom ( liste_struct_devises,
														     GINT_TO_POINTER ( echeance -> devise ),
														     (GCompareFunc) recherche_devise_par_no )->data)),
							 NOM_DU_COMPTE ));


	    gtk_misc_set_alignment ( GTK_MISC ( label ),
				     0,
				     0.5);
	    gtk_box_pack_start ( GTK_BOX ( GTK_BIN ( frame_etat_echeances_finies ) -> child ),
				 label,
				 FALSE,
				 TRUE,
				 5 );
	    gtk_widget_show (  label );


	}
	else
	{
	    GtkWidget *vbox;

	    vbox = gtk_vbox_new ( FALSE,
				  5 );
	    gtk_container_add ( GTK_CONTAINER ( frame_etat_echeances_finies ),
				vbox );
	    gtk_widget_show ( vbox );

	    label = gtk_label_new ("");
	    gtk_box_pack_start ( GTK_BOX ( vbox ),
				 label,
				 FALSE,
				 FALSE,
				 0 );
	    gtk_widget_show ( label );

	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + echeance ->compte;

	    if ( echeance -> montant >= 0 )
		label = gtk_label_new ( g_strdup_printf (PRESPACIFY(_("%4.2f %s credit on %s")),
							 echeance ->montant,
							 devise_name ((struct struct_devise *)(g_slist_find_custom ( liste_struct_devises,
														     GINT_TO_POINTER ( echeance -> devise ),
														     (GCompareFunc) recherche_devise_par_no )->data)),
							 NOM_DU_COMPTE ));
	    else
		label = gtk_label_new ( g_strdup_printf (PRESPACIFY(_("%4.2f %s debit on %s")),
							 -echeance ->montant,
							 devise_name ((struct struct_devise *)(g_slist_find_custom ( liste_struct_devises,
														     GINT_TO_POINTER ( echeance -> devise ),
														     (GCompareFunc) recherche_devise_par_no )->data)),
							 NOM_DU_COMPTE ));


	    gtk_misc_set_alignment ( GTK_MISC ( label ),
				     0,
				     0.5);
	    gtk_box_pack_start ( GTK_BOX ( vbox ),
				 label,
				 FALSE,
				 TRUE,
				 5 );
	    gtk_widget_show (  label );


	    gtk_widget_show ( frame_etat_echeances_finies );
	    /*  gtk_widget_show ( separateur_ech_finies_soldes_mini ); */
	}

	gsliste_echeances = g_slist_remove ( gsliste_echeances, 
					     echeance );
	free ( echeance );
	nb_echeances--;

	if ( ( echeance_selectionnnee = echeance ))
	    echeance_selectionnnee = gtk_clist_get_row_data ( GTK_CLIST ( liste_echeances ),
							      gtk_clist_find_row_from_data ( GTK_CLIST ( liste_echeances ),
											     echeance ) + 1);

    }
    else
    {
	echeance -> jour = echeance -> date -> day;
	echeance -> mois = echeance -> date -> month;
	echeance -> annee = echeance -> date -> year;
    }

    mise_a_jour_calendrier ();

    g_date_free ( date_suivante );

}
/******************************************************************************/

/******************************************************************************/
/* Fonction completion_operation_par_tiers_echeancier */
/* appel�e lorsque le tiers perd le focus */
/* r�cup�re le tiers, et recherche la derni�re op� associ�e � ce tiers */
/******************************************************************************/
void completion_operation_par_tiers_echeancier ( void )
{
    GSList *liste_tmp;
    struct struct_tiers *tiers;
    struct structure_operation *operation;
    gint no_compte;
    GSList *pointeur_ope;

    /* s'il y a quelque chose dans les cr�dit ou d�bit ou cat�gories, on se barre */

    if ( gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ) == style_entree_formulaire[ENCLAIR]
	 ||
	 gtk_widget_get_style ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ) == style_entree_formulaire[ENCLAIR]
	 ||
	 gtk_widget_get_style ( GTK_COMBOFIX (widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY])->entry ) == style_entree_formulaire[ENCLAIR] )
	return;


    liste_tmp = g_slist_find_custom ( liste_struct_tiers,
				      g_strstrip ( gtk_combofix_get_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_PARTY]))),
				      ( GCompareFunc ) recherche_tiers_par_nom );

    /*   si nouveau tiers,  on s'en va simplement */

    if ( !liste_tmp )
	return;

    tiers = liste_tmp -> data;


    /* on fait d'abord le tour du compte courant pour recherche une op� avec ce tiers */
    /* s'il n'y a aucune op� correspondante, on fait le tour de tous les comptes */

    no_compte = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT]  ) -> menu_item ),
							"no_compte" ) );
    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + no_compte;

    operation = NULL;
    pointeur_ope = LISTE_OPERATIONS;

    while ( pointeur_ope )
    {
	struct structure_operation *ope_test;

	ope_test = pointeur_ope -> data;

	if ( ope_test -> tiers == tiers -> no_tiers )
	{
	    if ( operation )
	    {
		if ( g_date_compare ( ope_test -> date,
				      operation -> date ) >= 0 )
		    operation = ope_test;
	    }
	    else
		operation = ope_test;
	}

	pointeur_ope = pointeur_ope -> next;
    }

    if ( !operation )
    {
	/* aucune op�ration correspondant � ce tiers n'a �t� trouv�e dans le compte courant */
	/*       on recherche dans les autres comptes, la premi�re trouv�e fera l'affaire */

	gint i;

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte;

	for ( i = 0 ; i < nb_comptes ; i++ )
	{
	    if ( i != no_compte )
	    {
		pointeur_ope = LISTE_OPERATIONS;

		while ( pointeur_ope )
		{
		    struct structure_operation *ope_test;

		    ope_test = pointeur_ope -> data;

		    if ( ope_test -> tiers == tiers -> no_tiers )
		    {
			operation = ope_test;
			pointeur_ope = NULL;
			i = nb_comptes;
		    }
		    else
			pointeur_ope = pointeur_ope -> next;
		}
	    }
	    p_tab_nom_de_compte_variable++;
	}
    }

    /* si on n'a trouv� aucune op�ration, on se tire */

    if ( !operation )
	return;

    /* remplit les diff�rentes entr�es du formulaire */

    /* remplit les montant */

    if ( operation -> montant < 0 )
    {
	entree_prend_focus ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_DEBIT] ),
			     g_strdup_printf ( "%4.2f",
					       -operation -> montant ));
	/* met le menu des types d�bits */

	if ( !etat.affiche_tous_les_types )
	{
	    GtkWidget *menu;

	    if ( (menu = creation_menu_types ( 1,
					       GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ) -> menu_item ),
										       "no_compte" )),
					       1 )))
	    {
		gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
					   menu );
		gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] );
	    }
	    else
		gtk_widget_hide ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] );
	}
    }
    else
    {
	entree_prend_focus ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_CREDIT] ),
			     g_strdup_printf ( "%4.2f",
					       operation -> montant ));
	/* met le menu des types cr�dits */

	if ( !etat.affiche_tous_les_types )
	{
	    GtkWidget *menu;

	    if ( (menu = creation_menu_types ( 2,
					       GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ) -> menu_item ),
										       "no_compte" )),
					       1 )))
	    {
		gtk_option_menu_set_menu ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
					   menu );
		gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] );
	    }
	    else
		gtk_widget_hide ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] );
	}
    }

    /* met la devise */

    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE] ),
				  g_slist_position ( liste_struct_devises,
						     g_slist_find_custom ( liste_struct_devises,
									   GINT_TO_POINTER ( operation -> devise ),
									   ( GCompareFunc ) recherche_devise_par_no )));

    /* mise en forme des cat�gories */

    /* v�rifie si c'est un virement */

    if ( operation -> relation_no_operation )
    {
	/* c'est un virement, on l'affiche */

	entree_prend_focus ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY]);

	p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> relation_no_compte;

	gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ),
				g_strconcat ( COLON(_("Transfer")),
					      NOM_DU_COMPTE,
					      NULL ));
    }
    else
    {
	liste_tmp = g_slist_find_custom ( liste_struct_categories,
					  GINT_TO_POINTER ( operation -> categorie ),
					  ( GCompareFunc ) recherche_categorie_par_no );

	if ( liste_tmp )
	{
	    GSList *liste_tmp_2;

	    entree_prend_focus ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY]);

	    liste_tmp_2 = g_slist_find_custom ( (( struct struct_categ * )( liste_tmp -> data )) -> liste_sous_categ,
						GINT_TO_POINTER ( operation -> sous_categorie ),
						( GCompareFunc ) recherche_sous_categorie_par_no );
	    if ( liste_tmp_2 )
		gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ),
					g_strconcat ( (( struct struct_categ * )( liste_tmp -> data )) -> nom_categ,
						      " : ",
						      (( struct struct_sous_categ * )( liste_tmp_2 -> data )) -> nom_sous_categ,
						      NULL ));
	    else
		gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_CATEGORY] ),
					(( struct struct_categ * )( liste_tmp -> data )) -> nom_categ );

	}
    }

    /* met l'option menu du type d'op� */

    if ( GTK_WIDGET_VISIBLE ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ))
    {
	gint place_type;

	place_type = cherche_no_menu_type_echeancier ( operation -> type_ope );

	/*       si la place est trouv�e, on la met, sinon on met � la place par d�faut */

	if ( place_type != -1 )
	    gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
					  place_type );
	else
	{
	    p_tab_nom_de_compte_variable = p_tab_nom_de_compte + operation -> no_compte;

	    if ( operation -> montant < 0 )
		place_type = cherche_no_menu_type_echeancier ( TYPE_DEFAUT_DEBIT );
	    else
		place_type = cherche_no_menu_type_echeancier ( TYPE_DEFAUT_CREDIT );

	    if ( place_type != -1 )
		gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
					      place_type );
	    else
	    {
		struct struct_type_ope *type;

		gtk_option_menu_set_history ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ),
					      0 );

		/*  on met ce type par d�faut, vu que celui par d�faut marche plus ... */

		if ( operation -> montant < 0 )
		    TYPE_DEFAUT_DEBIT = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) -> menu_item ),
										"no_type" ));
		else
		    TYPE_DEFAUT_CREDIT = GPOINTER_TO_INT ( gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) -> menu_item ),
										 "no_type" ));

		/* r�cup�re l'adr du type pour afficher l'entr�e si n�cessaire */

		type = gtk_object_get_data ( GTK_OBJECT ( GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ) -> menu_item ),
					     "adr_type" );

		if ( type -> affiche_entree )
		    gtk_widget_show ( widget_formulaire_echeancier[SCHEDULER_FORM_CHEQUE] );
	    }
	}
    }

    /* met en place l'exercice */

    gtk_option_menu_set_history (  GTK_OPTION_MENU ( widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] ),
				   cherche_no_menu_exercice ( operation -> no_exercice,
							      widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] ));

    /* met en place l'imputation budg�taire */


    liste_tmp = g_slist_find_custom ( liste_struct_imputation,
				      GINT_TO_POINTER ( operation -> imputation ),
				      ( GCompareFunc ) recherche_imputation_par_no );

    if ( liste_tmp )
    {
	GSList *liste_tmp_2;

	entree_prend_focus ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY]);

	liste_tmp_2 = g_slist_find_custom ( (( struct struct_imputation * )( liste_tmp -> data )) -> liste_sous_imputation,
					    GINT_TO_POINTER ( operation -> sous_imputation ),
					    ( GCompareFunc ) recherche_sous_categorie_par_no );
	if ( liste_tmp_2 )
	    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ),
				    g_strconcat ( (( struct struct_imputation * )( liste_tmp -> data )) -> nom_imputation,
						  " : ",
						  (( struct struct_sous_imputation * )( liste_tmp_2 -> data )) -> nom_sous_imputation,
						  NULL ));
	else
	    gtk_combofix_set_text ( GTK_COMBOFIX ( widget_formulaire_echeancier[SCHEDULER_FORM_BUDGETARY] ),
				    (( struct struct_imputation * )( liste_tmp -> data )) -> nom_imputation );
    }


    /*   remplit les notes */

    if ( operation -> notes )
    {
	entree_prend_focus ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] );
	gtk_entry_set_text ( GTK_ENTRY ( widget_formulaire_echeancier[SCHEDULER_FORM_NOTES] ),
			     operation -> notes );
    }
}
/******************************************************************************/

/******************************************************************************/
void degrise_formulaire_echeancier ( void )
{
    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_echeancier[SCHEDULER_FORM_DEVISE] ), TRUE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_echeancier[SCHEDULER_FORM_ACCOUNT] ), TRUE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_echeancier[SCHEDULER_FORM_TYPE] ), TRUE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_echeancier[SCHEDULER_FORM_EXERCICE] ), TRUE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_echeancier[SCHEDULER_FORM_MODE] ), TRUE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQUENCY] ), TRUE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( widget_formulaire_echeancier[SCHEDULER_FORM_FREQ_CUSTOM_MENU] ), TRUE );
    gtk_widget_set_sensitive ( GTK_WIDGET ( hbox_valider_annuler_echeance ), TRUE );
    gtk_widget_show ( label_saisie_modif );
}
/******************************************************************************/
