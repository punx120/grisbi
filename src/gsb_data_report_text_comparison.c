/* ************************************************************************** */
/* work with the struct of text comparison of the reports                     */
/*                                                                            */
/*                                  gsb_data_report_text_comparison           */
/*                                                                            */
/*     Copyright (C)    2000-2007 Cédric Auger (cedric@grisbi.org)            */
/*          2003-2007 Benjamin Drieu (bdrieu@april.org)                       */
/*          2008-2021 Pierre Biava (grisbi@pierre.biava.name)                 */
/*         https://www.grisbi.org/                                            */
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
 * \file gsb_data_report_text_comparison.c
 * work with the text comparison structure, no GUI here
 */


#include "config.h"

#include "include.h"
#include <string.h>

/*START_INCLUDE*/
#include "gsb_data_report_text_comparison.h"
#include "utils_str.h"
/*END_INCLUDE*/

/** \struct
 * describe a report text comparison
 **/

typedef struct _TextComparaison	TextComparaison;

struct _TextComparaison {
    /** @name general stuff */
    gint text_comparison_number;
    gint report_number;				/**< the associated report */

    /** @name saved values of the structure */
    gint 		link_to_last_text_comparison;	/**< -1=first comparison, 0=and, 1=or, 2=except */
    gint 		field;	/**<	0=payee, 1=the payee description(in payee page), 2=categ, 3=sub categ, 4=budget,
								5=sub budget, 6=note, 7=bank ref, 8=voucher, 9=chq, 10=statement */

    /** @name text comparison */
    gint 		operator;		/**< 0=contain, 1=don't contain, 2=begin with, 3=end with, 4=empty, 5=no empty */
    gchar *		text;
    gint 		use_text;		/**< for the field with number (cheque, voucher...), if TRUE they are used as text */

    /** @name cheque comparison */
    gint 		first_comparison;			/**< 0= =, 1= <, 2= <=, 3= >, 4= >=, 5= !=, 6= =0, 7= !=0, 8= >0, 9= <0 */
    gint 		first_amount;
    gint 		link_first_to_second_part;	/**< 0=and, 1=or, 2=except, 3=none */
    gint 		second_comparison;			/**< 0= =, 1= <, 2= <=, 3= >, 4= >=, 5= != */
    gint 		second_amount;

    /** @name dynamic values used for the gui */
	gpointer	widget;
};


/*START_EXTERN*/
/*END_EXTERN*/

/*START_STATIC*/
/** contains a g_slist of TextComparaison */
static GSList *text_comparison_list = NULL;

/** a pointers to the last text comparison used (to increase the speed) */
static TextComparaison *text_comparison_buffer;
/*END_STATIC*/

/******************************************************************************/
/* Private functions                                                          */
/******************************************************************************/
/**
 * This function is called to free the memory used by a TextComparaison structure
 *
 * \param
 *
 * \return
 **/
static void _gsb_data_report_text_comparison_free (TextComparaison* text_comparison)
{
	if (!text_comparison)
		return;

	if (text_comparison->text)
		g_free (text_comparison->text);
	g_free (text_comparison);
    if (text_comparison_buffer == text_comparison)
		text_comparison_buffer = NULL;
}

/**
 * return a pointer on the text comparison structure which the number is in the parameter.
 * that structure is stored in the buffer
 *
 * \param text_comparison_number
 *
 * \return a pointer to the text_comparison, NULL if not found
 **/
static TextComparaison *gsb_data_report_text_comparison_get_struct_by_no (gint text_comparison_number)
{
    GSList *text_comparison_list_tmp;

    /* check first if the text_comparison is in the buffer */

    if (text_comparison_buffer
		&& text_comparison_buffer->text_comparison_number == text_comparison_number)
		return text_comparison_buffer;

    text_comparison_list_tmp = text_comparison_list;

    while (text_comparison_list_tmp)
    {
		TextComparaison *text_comparison;

		text_comparison = text_comparison_list_tmp->data;

		if (text_comparison->text_comparison_number == text_comparison_number)
		{
			text_comparison_buffer = text_comparison;

			return text_comparison;
		}

		text_comparison_list_tmp = text_comparison_list_tmp->next;
    }

    /* here, we didn't find any text_comparison with that number */
    return 0;
}

/** find and return the last number of text_comparison
 *
 * \param none
 *
 * \return last number of text_comparison
 **/
static gint gsb_data_report_text_comparison_max_number (void)
{
    GSList *tmp;
    gint number_tmp = 0;

    tmp = text_comparison_list;

    while (tmp)
    {
		TextComparaison *text_comparison;

		text_comparison = tmp->data;

		if (text_comparison->text_comparison_number > number_tmp)
			number_tmp = text_comparison->text_comparison_number;

		tmp = tmp->next;
    }

	return number_tmp;
}

/******************************************************************************/
/* Public functions                                                           */
/******************************************************************************/
/**
 * set the text comparison global variables to NULL, usually when we init all the global variables
 *
 * \param none
 *
 * \return FALSE
 **/
gboolean gsb_data_report_text_comparison_init_variables (void)
{
	if (text_comparison_list)
    {
        GSList *tmp_list = text_comparison_list;

		while (tmp_list)
        {
			TextComparaison *text_comparison;

			text_comparison = tmp_list-> data;
			tmp_list = tmp_list->next;
			_gsb_data_report_text_comparison_free (text_comparison);
        }
        g_slist_free (text_comparison_list);
    }
    text_comparison_list = NULL;
    text_comparison_buffer = NULL;

	return FALSE;
}

/**
 * create a new text_comparison with a number, append it to the list
 * and return the number
 *
 * \param number the number we want to give to that text_comparison, if 0, give a new number by itself
 *
 * \return the number of the new text_comparison
 **/
gint gsb_data_report_text_comparison_new (gint number)
{
    TextComparaison *text_comparison;

    text_comparison = g_malloc0 (sizeof (TextComparaison));

	if (number)
		text_comparison->text_comparison_number = number;
	else
		text_comparison->text_comparison_number = gsb_data_report_text_comparison_max_number () + 1;


    text_comparison_list = g_slist_append (text_comparison_list,
					    text_comparison);

    text_comparison_buffer = text_comparison;

    return text_comparison->text_comparison_number;
}

/**
 * remove a text_comparison
 *
 * \param text_comparison_number the text_comparison we want to remove
 *
 * \return TRUE ok
 **/
gboolean gsb_data_report_text_comparison_remove (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison_list = g_slist_remove (text_comparison_list, text_comparison);
    _gsb_data_report_text_comparison_free (text_comparison);

    return TRUE;
}

/**
 * dupplicate an text_comparison
 * and return the number
 *
 * \param number the number we want to dup to that text_comparison
 *
 * \return the number of the new text_comparison
 **/
gint gsb_data_report_text_comparison_dup (gint last_text_comparison_number)
{
    TextComparaison *text_comparison;
    TextComparaison *last_text_comparison;
    gint text_comparison_number;

    last_text_comparison = gsb_data_report_text_comparison_get_struct_by_no (last_text_comparison_number);
	if (!last_text_comparison)
		return 0;

    text_comparison_number = gsb_data_report_text_comparison_new (0);
    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);
	if (NULL == text_comparison)
		return 0;

    text_comparison->report_number = last_text_comparison->report_number;
    text_comparison->link_to_last_text_comparison = last_text_comparison->link_to_last_text_comparison;
    text_comparison->field = last_text_comparison->field;
    text_comparison->operator = last_text_comparison->operator;

	if (last_text_comparison->text)
		text_comparison->text = my_strdup (last_text_comparison->text);
    text_comparison->use_text = last_text_comparison->use_text;
    text_comparison->first_comparison = last_text_comparison->first_comparison;
    text_comparison->first_amount = last_text_comparison->first_amount;
    text_comparison->link_first_to_second_part = last_text_comparison->link_first_to_second_part;
    text_comparison->second_comparison = last_text_comparison->second_comparison;
    text_comparison->second_amount = last_text_comparison->second_amount;

    return text_comparison_number;
}

/**
 * set the report_number
 *
 * \param text_comparison_number number of the text_comparison
 * \param report_number
 *
 * \return TRUE if ok
 **/
gboolean gsb_data_report_text_comparison_set_report_number (gint text_comparison_number,
															gint report_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison->report_number = report_number;

	return TRUE;
}

/**
 * get the link_to_last_text_comparison
 *
 * \param text_comparison_number the number of the text_comparison
 *
 * \return the  of the text_comparison, -1 if problem
 **/
gint gsb_data_report_text_comparison_get_link_to_last_text_comparison (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return 0;

    return text_comparison->link_to_last_text_comparison;
}

/**
 * set the link_to_last_text_comparison
 *
 * \param text_comparison_number number of the text_comparison
 * \param link_to_last_text_comparison
 *
 * \return TRUE if ok
 **/
gboolean gsb_data_report_text_comparison_set_link_to_last_text_comparison (gint text_comparison_number,
																		   gint link_to_last_text_comparison)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison->link_to_last_text_comparison = link_to_last_text_comparison;

    return TRUE;
}

/**
 * get the field
 *
 * \param text_comparison_number the number of the text_comparison
 *
 * \return the field of the text_comparison, -1 if problem
 **/
gint gsb_data_report_text_comparison_get_field (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return 0;

    return text_comparison->field;
}

/**
 * set the field
 *
 * \param text_comparison_number number of the text_comparison
 * \param field
 *
 * \return TRUE if ok
 **/
gboolean gsb_data_report_text_comparison_set_field (gint text_comparison_number,
													gint field)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison->field = field;

    return TRUE;
}

/**
 * get the operator
 *
 * \param text_comparison_number the number of the text_comparison
 *
 * \return the operator of the text_comparison, -1 if problem
 **/
gint gsb_data_report_text_comparison_get_operator (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return 0;

    return text_comparison->operator;
}

/**
 * set the operator
 *
 * \param text_comparison_number number of the text_comparison
 * \param operator
 *
 * \return TRUE if ok
 **/
gboolean gsb_data_report_text_comparison_set_operator (gint text_comparison_number,
													   gint operator)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison->operator = operator;

    return TRUE;
}

/**
 * get the text
 *
 * \param text_comparison_number the number of the text_comparison
 *
 * \return the text of the text_comparison, -1 if problem
 **/
gchar *gsb_data_report_text_comparison_get_text (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return NULL;

    return text_comparison->text;
}

/**
 * set the text
 *
 * \param text_comparison_number number of the text_comparison
 * \param text  the value will be dumped before added so can free after
 * 		if the length is 0, set NULL
 *
 * \return TRUE if ok
 **/
gboolean gsb_data_report_text_comparison_set_text (gint text_comparison_number,
												   const gchar *text)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

	if (text && strlen (text))
		text_comparison->text = my_strdup (text);
    else
		text_comparison->text = NULL;

    return TRUE;
}

/**
 * get the use_text
 *
 * \param text_comparison_number the number of the text_comparison
 *
 * \return the use_text of the text_comparison, -1 if problem
 **/
gint gsb_data_report_text_comparison_get_use_text (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return 0;

    return text_comparison->use_text;
}

/**
 * set the use_text
 *
 * \param text_comparison_number number of the text_comparison
 * \param use_text
 *
 * \return TRUE if ok
 **/
gboolean gsb_data_report_text_comparison_set_use_text (gint text_comparison_number,
													   gint use_text)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison->use_text = use_text;

    return TRUE;
}

/**
 * get the first_comparison
 *
 * \param text_comparison_number the number of the text_comparison
 *
 * \return the  of the text_comparison, -1 if problem
 **/
gint gsb_data_report_text_comparison_get_first_comparison (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return 0;

    return text_comparison->first_comparison;
}

/**
 * set the first_comparison
 *
 * \param text_comparison_number number of the text_comparison
 * \param first_comparison
 *
 * \return TRUE if ok
 **/
gboolean gsb_data_report_text_comparison_set_first_comparison (gint text_comparison_number,
															   gint first_comparison)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison->first_comparison = first_comparison;

    return TRUE;
}

/**
 * get the first_amount
 *
 * \param text_comparison_number the number of the text_comparison
 *
 * \return the first_amount of the text_comparison, -1 if problem
 **/
gint gsb_data_report_text_comparison_get_first_amount (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return 0;

    return text_comparison->first_amount;
}

/**
 * set the first_amount
 *
 * \param text_comparison_number number of the text_comparison
 * \param first_amount
 *
 * \return TRUE if ok
 **/
gboolean gsb_data_report_text_comparison_set_first_amount (gint text_comparison_number,
														   gint first_amount)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison->first_amount = first_amount;

    return TRUE;
}

/**
 * get the link_first_to_second_part
 *
 * \param text_comparison_number the number of the text_comparison
 *
 * \return the  of the text_comparison, -1 if problem
 **/
gint gsb_data_report_text_comparison_get_link_first_to_second_part (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return 0;

    return text_comparison->link_first_to_second_part;
}

/**
 * set the link_first_to_second_part
 *
 * \param text_comparison_number number of the text_comparison
 * \param link_first_to_second_part
 *
 * \return TRUE if ok
 **/
gboolean gsb_data_report_text_comparison_set_link_first_to_second_part (gint text_comparison_number,
																		gint link_first_to_second_part)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison->link_first_to_second_part = link_first_to_second_part;

    return TRUE;
}

/**
 * get the second_comparison
 *
 * \param text_comparison_number the number of the text_comparison
 *
 * \return the  of the text_comparison, -1 if problem
 **/
gint gsb_data_report_text_comparison_get_second_comparison (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return 0;

    return text_comparison->second_comparison;
}

/**
 * set the second_comparison
 *
 * \param text_comparison_number number of the text_comparison
 * \param second_comparison
 *
 * \return TRUE if ok
 **/
gboolean gsb_data_report_text_comparison_set_second_comparison (gint text_comparison_number,
																gint second_comparison)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison->second_comparison = second_comparison;

    return TRUE;
}

/**
 * get the second_amount
 *
 * \param text_comparison_number the number of the text_comparison
 *
 * \return the second_amount of the text_comparison, -1 if problem
 **/
gint gsb_data_report_text_comparison_get_second_amount (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return 0;

    return text_comparison->second_amount;
}

/**
 * set the second_amount
 *
 * \param text_comparison_number number of the text_comparison
 * \param second_amount
 *
 * \return TRUE if ok
 **/
gboolean gsb_data_report_text_comparison_set_second_amount (gint text_comparison_number,
															gint second_amount)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison->second_amount = second_amount;

    return TRUE;
}

/**
 *
 *
 * \param
 *
 * \return
 **/
gpointer gsb_data_report_text_comparison_get_widget (gint text_comparison_number)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return 0;

    return text_comparison->widget;
}

/**
 *	memorise le widget associé a text_comparison_number
 *
 * \param 		text_comparison_number
 * \param		widget
 *
 * \return TRUE or FALSE
 **/
gboolean gsb_data_report_text_comparison_set_widget (gint text_comparison_number,
													 gpointer widget)
{
    TextComparaison *text_comparison;

    text_comparison = gsb_data_report_text_comparison_get_struct_by_no (text_comparison_number);

	if (!text_comparison)
		return FALSE;

    text_comparison->widget = widget;

    return TRUE;
}

/**
 *
 *
 * \param
 *
 * \return
 **/
/* Local Variables: */
/* c-basic-offset: 4 */
/* End: */
