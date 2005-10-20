/* ************************************************************************** */
/*                                                                            */
/*     Copyright (C)	     2005 Benjamin Drieu (bdrieu@april.org)	      */
/* 			http://www.grisbi.org				      */
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

/*START_INCLUDE*/
#include "csv_parse.h"
#include "utils_devises.h"
#include "utils_dates.h"
#include "utils_str.h"
#include "import.h"
#include "structures.h"
/*END_INCLUDE*/

/*START_EXTERN*/
extern FILE * out;
/*END_EXTERN*/

/*START_STATIC*/
static gchar * sanitize_field ( gchar * begin, gchar * end  );
/*END_STATIC*/



/**
 * TODO
 *
 *
 */
GSList * csv_parse_line ( gchar * contents[], gchar * separator )
{
    gchar * tmp = (* contents), * begin = tmp;
    gint is_unquoted = FALSE, len = strlen ( separator );
    GSList * list = NULL;

    if ( *tmp == '\n' )
    {
	*contents = tmp+1;
	return GINT_TO_POINTER(-1);
    }
    
    if ( *tmp == '!' || *tmp == '#' || *tmp == ';' )
    {
	*contents = strchr ( tmp, '\n' ) + 1;
	return GINT_TO_POINTER(-1);
    }

    while ( *tmp )
    {
	switch ( *tmp )
	{
	    case '\n':
		list = g_slist_append ( list, sanitize_field ( begin, tmp ) );
		*contents = tmp+1;
		return list;

	    case '"':
		if ( ! is_unquoted )
		{
		    tmp++;
		    while ( *tmp )
		    {
			/* Thsi is lame escaping but we need to
			 * support it. */
			if ( *tmp == '\\' && *(tmp+1) == '"' )
			{
			    tmp += 2;
			}

			/* End of quoted string. */
			if ( *tmp == '"' && *(tmp+1) != '"' )
			{
			    tmp += 1;
			    break;
			}
			
			tmp++;
		    }
		}

	    default:
		is_unquoted = TRUE;
		if ( !strncmp ( tmp, separator, len ) )
		{
		    list = g_slist_append ( list, sanitize_field ( begin, tmp ) );
		    begin = tmp + len;
		    is_unquoted = FALSE;
		}
		break;
	}

	tmp++;
    }

    return NULL;
}



/**
 * TODO
 *
 */
gchar * sanitize_field ( gchar * begin, gchar * end  )
{
    gchar * field, * iter;

    g_return_val_if_fail ( begin <= end, NULL );

    if ( begin == end )
	return "";

    iter = field = g_malloc ( end - begin );

    /* Strip out intial white spaces. */
    while ( *begin == ' ' )
	begin++;

    if ( *begin == '"' )
    {
	begin++;
	while ( *end != '"' && end >= begin )
	    end--;
    }

    while ( begin < end )
    {
	if ( *begin == '"' && *(begin+1) == '"' )
	    begin++;

	if ( *begin == '\\' && *(begin+1) == '"' )
	    begin++;
	
	*iter++ = *begin++;
    }

    /* Strip out remaining white spaces. */
    while ( *(iter-1) == ' ' || *(iter-1) == '\n' )
	iter--;

    *iter = '\0';

    return field;
}






/**
 *
 *
 */
gboolean csv_import_validate_string ( gchar * string )
{
    g_return_val_if_fail ( string, FALSE );

    return string != NULL;
}



/**
 *
 *
 */
gboolean csv_import_validate_date ( gchar * string )
{
    GDate * date;
    g_return_val_if_fail ( string, FALSE );
    
    date = gsb_parse_date_string ( string );
    return date && g_date_valid ( date );
}



/**
 *
 *
 */
gboolean csv_import_validate_number ( gchar * string )
{
    g_return_val_if_fail ( string, FALSE );

    while ( *string )
    {
	if ( ! g_ascii_isdigit ( * string ) ||
	     ! g_ascii_isspace ( * string ) )
	{
	    return FALSE;
	}

	string ++;
    }

    return TRUE;
}



/**
 *
 * We don't check punctuation signs because mileage may vary with
 * locales.
 */
gboolean csv_import_validate_amount ( gchar * string )
{
    g_return_val_if_fail ( string, FALSE );

    while ( *string )
    {
	if ( g_ascii_isalpha ( * string ) )
	{
	    return FALSE;
	}

	string++;
    }

    return TRUE;
}



/**
 *
 *
 */
gboolean csv_import_parse_currency ( struct struct_ope_importation * ope, gchar * string )
{
    struct struct_devise * currency;

    g_return_val_if_fail ( string, FALSE );

    currency = devise_par_nom ( string );
    if ( ! currency )
    {
	return FALSE;
    }

    return currency -> no_devise;
}



/**
 *
 *
 */
gboolean csv_import_parse_date ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, FALSE );

    ope -> date = gsb_parse_date_string ( string );
    if ( ! ope -> date )
    {
	ope -> date = gdate_today ();
    }

    return g_date_valid ( ope -> date );
}



/**
 *
 *
 */
gboolean csv_import_parse_value_date ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, FALSE );
    ope -> date_de_valeur = gsb_parse_date_string ( string );
    return g_date_valid ( ope -> date_de_valeur );
}



/**
 *
 *
 */
gboolean csv_import_parse_payee ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, FALSE );
    ope -> tiers = g_strdup ( string );
    return TRUE;
}



/**
 *
 *
 */
gboolean csv_import_parse_notes ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, FALSE );
    ope -> notes = g_strdup ( string );
    return TRUE;
}



/**
 *
 *
 */
gboolean csv_import_parse_voucher ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, FALSE );
    ope -> cheque = my_strtod ( string, NULL );
    return TRUE;
}



/**
 *
 *
 */
gboolean csv_import_parse_category ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, FALSE );
    ope -> categ = g_strdup ( string );
    return TRUE;
}



/**
 *
 *
 */
gboolean csv_import_parse_sub_category ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, FALSE );
    if ( ! ope -> categ || ! strlen ( string ) )
	return FALSE;
    ope -> categ = g_strconcat ( ope -> categ, " : ", string, NULL );
    return TRUE;
}



/**
 * TODO: Use  a real parsing
 *
 */
gboolean csv_import_parse_amount ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, FALSE );
    if ( strlen ( string ) > 0 )
    {
	ope -> montant = my_strtod ( string, NULL );
    }
    return TRUE;
}



/**
 *
 *
 */
gboolean csv_import_parse_credit ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, FALSE );
    if ( strlen ( string ) > 0 )
    {
	ope -> montant = my_strtod ( string, NULL );
    }
    return TRUE;
}



/**
 *
 *
 */
gboolean csv_import_parse_debit ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, FALSE );

    if ( strlen ( string ) > 0 )
    {
	ope -> montant = - my_strtod ( string, NULL );
    }
    return TRUE;
}



/**
 *
 *
 */
gboolean csv_import_parse_p_r ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, FALSE );

    if ( ! strcmp ( string, "P" ) )
    {
	ope -> p_r = OPERATION_POINTEE;
	return TRUE;
    }
    else if ( ! strcmp ( string, "T" ) )
    {
	ope -> p_r = OPERATION_TELERAPPROCHEE;
	return TRUE;
    }
    else if ( ! strcmp ( string, "R" ) )
    {
	ope -> p_r = OPERATION_RAPPROCHEE;
	return TRUE;
    }
    else if ( ! strlen ( string ) )
    {
	ope -> p_r = OPERATION_NORMALE;
	return TRUE;
    }
    return FALSE;
}



/**
 *
 *
 */
gboolean csv_import_parse_breakdown ( struct struct_ope_importation * ope, gchar * string )
{
    g_return_val_if_fail ( string, NULL );

    if ( ! strcmp ( string, "V" ) )
    {
	ope -> operation_ventilee = 1;
    }

    return TRUE;
}



/* Local Variables: */
/* c-basic-offset: 4 */
/* End: */
