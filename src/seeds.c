/*
    Copyright (C) <2010> <Alexandre Xavier Falcão and Thiago Vallin Spina>

    This file is part of IFT-demo.

    IFT-demo is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    IFT-demo is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with IFT-demo.  If not, see <http://www.gnu.org/licenses/>.

    please see full copyright in COPYING file.
    -------------------------------------------------------------------------

    written by A.X. Falcão <afalcao@ic.unicamp.br> and by T.V. Spina
    <tvspina@liv.ic.unicamp.br>, 2010

*/

#include "seeds.h"

void ReadSeeds(char *filename, Set **Obj, Set **Bkg)
{
  FILE *fp=fopen(filename,"r");
  int i,x,y,l,mk,nseeds, ncols, nrows;

  if(fscanf(fp,"%d %d %d",&nseeds, &ncols, &nrows)!=0);

  for (i=0; i < nseeds; i++){
    if(fscanf(fp,"%d %d %d %d",&x,&y,&mk,&l)!=0);
    if (l==0)
      InsertSet(Bkg, x + ncols*y);
    else
      InsertSet(Obj, x + ncols*y);
  }
  fclose(fp);
}
