/*
SText
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

/*
Sihabul Milah
08/12/05 17:17

29-03-2006
tambah int GetTextStartAtText (const char *longText, char *text, int len, const char *atText, int start_pos)


13/12/05 11:55
Add some functions
*/

#include <string.h>
#include <stdlib.h>

int ReplaceChar (char* wholeText, char oldC, char newC)
{
  int c_char = 0;
  char *temp;

  /*
  if (!newC)
    return 0;
  */

  temp = wholeText;
  while ( (temp=strchr(temp,oldC)) )
  {
    *temp = newC;
    c_char++;
  }

  //strcpy (wholeText, temp);
  return c_char;
}

int RemoveChar (char* wholeText, char rmC)
{
  int c_char = 0;
  char *temp, *t1;
  char *orgText;
  int p;

  if(!wholeText && !rmC)
    return 0;
  if(!wholeText[0])
    return 0;

  //orgText = (char *)malloc(sizeof(wholeText));
  orgText = (char *)malloc(strlen(wholeText)+1);
  if (!orgText)
    return 0;

  strcpy(orgText, wholeText);

  temp = orgText;
  wholeText[0] = 0;
  while ( (t1 = strchr(temp, rmC)) )
  {
     p = t1 - temp;

     c_char++;
     if (p>0)
     {
       if (wholeText[0]==0)
       {
         strncpy (wholeText, temp, p);
         wholeText[p] = 0;
       }
       else
       {
         temp[p] = 0;
         strncat (wholeText, temp, p+1);
       }
     }
     temp = t1;
     temp++;
   }
   if (wholeText[0]==0)
     strcpy (wholeText, temp);
   else
     strcat (wholeText, temp);

   if (orgText)
     free(orgText);
   return c_char;
}


int RemoveText (char* wholeText, char *rmText)
{
  int c_text = 0;
  char *temp, *t1;
  char *orgText;
  int p,len;

  if(!wholeText || !rmText)
    return 0;
  if(!wholeText[0] || !rmText[0])
    return 0;

  //orgText = (char *)malloc(sizeof(wholeText));
  orgText = (char *)malloc(strlen(wholeText)+1);
  if (!orgText)
    return 0;

  strcpy(orgText, wholeText);

  temp = orgText;
  wholeText[0]=0;
  len = strlen(rmText);

  while ( (t1 = strstr(temp, rmText)) )
  {
     p = t1 - temp;

     c_text++;
     if (p>0)
     {
       if (wholeText[0]==0)
       {
         strncpy (wholeText, temp, p);
         wholeText[p] = 0;
       }
       else
       {
         temp[p] = 0;
         strncat (wholeText, temp, p+1);
       }
     }
     temp = t1;
     temp+=len;
   }
   if (wholeText[0]==0)
     strcpy (wholeText, temp);
   else
     strcat (wholeText, temp);

   return c_text;
}

/*
GetLine : Get a line text in pszFileText by 0xd (LF) delimiter
*/

int GetLine(char *pszFileText, char *pszLineText, int *pos)
{
   int bSuccess = 0;
   char *temp;
   char *t1;
   int tpos = *pos;
   int len;

   pszLineText[0] = 0;
   temp = pszFileText;
   temp += tpos;
   while (temp[0]==0xd)
   {
         tpos++;
         temp++;
   }
   if (temp[0])
   {
      if ( (t1 = strchr (temp, 0xd)) )
      {
         len = t1-temp;
         tpos += (len+1);
         strncpy (pszLineText, temp, len);
         pszLineText[len] = 0;
      }
      else
          strcpy (pszLineText, temp);

      bSuccess = 1;
   }
   *pos = tpos;

   return bSuccess;
}

/*
LeftText : Get left text from longText
*/
int LeftText (char *longText, char *left, int len)
{
  int longlen = strlen(longText);
  if (longlen<len)
    len = longlen;
  strncpy(left, longText, len);
  left[len] = 0;
  return len;
}

/*
MidText : Get mid text from longText
*/
int MidText (char *longText, char *mid, int start, int len)
{
  char *temp;
  int longlen = strlen(longText);

  if (longlen>start)
  {
    if (longlen<(start+len))
      len = longlen-start;

    temp = longText;
    temp+=start;
    strncpy(mid, temp, len);
    mid[len] = 0;
  }
  else
    len = 0;

  return len;
}

/*
RightText : Get right text from longText
*/
int RightText (char *longText, char *right, int len)
{
  int longlen = strlen(longText);
  char *temp;

  if (len>longlen)
    len = longlen;

  temp = longText;
  temp += (longlen-len);
  strncpy(right, temp, len);
  right[len] = 0;

  return len;
}

void TrimLeft (char *longText)
{
  char *temp;

  temp = longText;
  while (temp[0] == ' ')
  {
    temp++;
  }

  strcpy (longText, temp);
}

void TrimRight (char *longText)
{
  int longlen = strlen(longText);

  if (!longlen)
    return;
  while (longText[longlen-1]==' ')
  {
    longlen--;
    longText[longlen] = 0;
  }
}

void UpperText (char *text)
{
  int i;

  i = 0;
  while (text[i])
  {
     if (text[i]>='a' && text[i]<='z')
       text[i] = (text[i]-'a'+'A');
     i++;
  }
}

void LowerText (char *text)
{
   int i;

  i = 0;
  while (text[i])
  {
     if (text[i]>='A' && text[i]<='Z')
       text[i] = (text[i]-'A'+'a');
     i++;
  }
}

int GetTextUntilAChar (char *longText, char *text, int len, char aChar, int *pos)
{
  char *temp, *t1;
  int l= -1;
  int m;

  temp = longText;
  temp += (*pos);

  text[0] = 0;
  if ( (t1 = strchr(temp, aChar))!=NULL )
  {
    l = t1 - temp;
    m = l;
    if (l>len)
      l = len;
    strncpy(text, temp, l);
    text[l] = 0;
    (*pos) += (m+1);    /* 1 is aChar it self */
  }
  else
	strcpy (text, temp);

  return l;
}

void CopyText (char *dest, const char *src)
{
  int len = (sizeof(src)>sizeof(dest))?sizeof(dest):sizeof(src);
  memcpy(dest,src,len);
}

int GetTextStartAtText (const char *longText, char *text, int len, const char *atText, int start_pos)
{
	char *temp1, *temp2, *temp0;
	int l=len;

	temp0 = temp1 = (char *)longText;
	temp1 += start_pos;

	if ( (temp2=strstr(temp1, atText))==NULL)
	{
		l = strlen(temp1);
		if (l>len)
			l = len;
		strncpy (text, temp1, l);
		text[l] = 0;
		return -1;
	}
	else
	{
		l = strlen(temp2);
		if (l>len)
			l = len;
		strncpy (text, temp2, l);
		text[l] = 0;
		return (temp2-temp0);
	}
	return -1;
}
