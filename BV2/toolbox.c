/*
    BeeVee: Board viewer application - ECAD Layout reader like.
    Copyright (C) 2004-2007  Sihabul Milah

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Sihabul Milah
    http://eshabe.wordpress.com http://eshabe.blogspot.com
    eshabe@gmail.com
    Perumahan Bina Ummah B-15
    Kibing - Batu Aji
    Kota Batam - 29432
    INDONESIA
*/

#include <windows.h>

int AddListBoxItem (HWND ctrl, LPCSTR text_item, int data_item)
{
	int curItem = SendMessage (ctrl, LB_ADDSTRING, 0, (LPARAM)text_item);
	SendMessage (ctrl, LB_SETITEMDATA, (WPARAM)curItem, (LPARAM)data_item);
	return curItem;
}


LRESULT SelectListBoxIndex (HWND ctrl, int index)
{
	return SendMessage (ctrl, LB_SETCURSEL, index, 0);
}

LRESULT SelectListBoxString (HWND ctrl, char *string)
{
	return SendMessage (ctrl, LB_SELECTSTRING, -1, (LPARAM)string);
}

LRESULT GetListBoxCurSelString (HWND ctrl, LPCSTR theString)
{
	int curItem = SendMessage (ctrl, LB_GETCURSEL, 0,0);
	if (curItem != LB_ERR)
	{
		//int len;
		//len = SendMessage (ctrl, LB_GETTEXTLEN, curItem, 0);
		SendMessage (ctrl, LB_GETTEXT, curItem, (LPARAM)(LPCSTR)theString);
		//theString[len] = 0L;
	}
	return curItem;
}

int GetListBoxCurSelItemData (HWND ctrl, int *data)
{
	int curItem = SendMessage (ctrl, LB_GETCURSEL, 0,0);
	if (curItem != LB_ERR)
		*data = SendMessage (ctrl, LB_GETITEMDATA, curItem, 0);
	return curItem;
}

int GetListBoxCurSelItemDataAndString (HWND ctrl, int *data, char *string)
{
	int curItem = SendMessage (ctrl, LB_GETCURSEL, 0,0);
	if (curItem != LB_ERR)
	{
		*data = SendMessage (ctrl, LB_GETITEMDATA, curItem, 0);
		SendMessage (ctrl, LB_GETTEXT, curItem, (LPARAM)(LPCSTR)string);
	}
	return curItem;
}

/* IndexStart (zero-based index) = -1 to search from top */
int FindListBoxString (HWND ctrl, LPCSTR PrefixStr, int IndexStart)
{
	return SendMessage (ctrl, LB_FINDSTRING, (WPARAM)IndexStart, (LPARAM)PrefixStr);
}

int FindListBoxStringFromCurrent (HWND ctrl, LPCSTR PrefixStr)
{
	int curItem = SendMessage (ctrl, LB_GETCURSEL, 0,0);
	if (curItem != LB_ERR)
		return FindListBoxString (ctrl, PrefixStr, curItem);
	else
		return LB_ERR;
}

int FindListBoxStringExact (HWND ctrl, LPCSTR theString, int IndexStart)
{
	return SendMessage (ctrl, LB_FINDSTRINGEXACT, (WPARAM)IndexStart, (LPARAM)theString);
}

int AddComboBoxItem (HWND ctrl, LPCSTR text_item, int data_item)
{
	int curItem = SendMessage (ctrl, CB_ADDSTRING, 0, (LPARAM)text_item);
	SendMessage (ctrl, CB_SETITEMDATA, (WPARAM)curItem, (LPARAM)data_item);
	return curItem;
}

LRESULT SelectComboBoxIndex (HWND ctrl, int index)
{
	return SendMessage (ctrl, CB_SETCURSEL, index, 0);
}

LRESULT SelectComboBoxString (HWND ctrl, LPCSTR string)
{
	return SendMessage (ctrl, CB_SELECTSTRING, 0, (LPARAM)string);
}

int GetComboBoxCurSelItemData (HWND ctrl, int *data)
{
	int curItem = SendMessage (ctrl, CB_GETCURSEL, 0,0);
	if (curItem != LB_ERR)
		*data = SendMessage (ctrl, CB_GETITEMDATA, curItem, 0);
	return curItem;
}
