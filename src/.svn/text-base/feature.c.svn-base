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

#include "feature.h"

Features* CreateFeatures(int ncols, int nrows, int nfeats)
{
    Features *f=(Features *)calloc(1,sizeof(Features));

    f->ncols  = ncols;
    f->nrows  = nrows;
    f->nelems = ncols*nrows;
    f->elem   = (FElem *)calloc(f->nelems,sizeof(FElem));
    f->nfeats = nfeats;
    f->Imax = 0;

    int i;

    for (i=0; i < f->nelems; i++)
        f->elem[i].feat = AllocFloatArray(nfeats);

    return f;
}

void DestroyFeatures(Features **f)
{
    int i;

    if ((*f)!=NULL)
    {
        for (i=0; i < (*f)->nelems; i++)
            free((*f)->elem[i].feat);
        free((*f)->elem);
        free(*f);
        (*f)=NULL;
    }
}

Features* CopyFeatures(Features* feat)
{
    if(feat == NULL)
    {
        Error("Features* feat NULL ","CopyFeatures");
    }

    Features* result = CreateFeatures(feat->ncols, feat->nrows, feat->nfeats);

    result->Imax = feat->Imax;

    int i;
    int size = feat->nfeats*sizeof(float);

    for(i = 0; i < feat->nelems; i++)
        memcpy(result->elem[i].feat, feat->elem[i].feat,size);

    return result;
}

// Using Linear Convolution with Gaussian filters

Features *GaussImageFeats(Image *img, int nscales)
{
    Features *f=CreateFeatures(img->ncols, img->nrows, nscales);
    AdjRel   *A=NULL;
    int       s,i,p,q;
    Pixel     u,v;
    float    *w,d,K1,K2,sigma2,val,sum;

    f->Imax = MaximumValue(img);

    for (s=1; s <= nscales; s=s+1)
    {
        A  = Circular(s);
        w  = AllocFloatArray(A->n);
        sigma2 = (s/3.0)*(s/3.0);
        K1     =  2.0*sigma2;
        K2     = 1.0/sqrt(2.0*PI*sigma2);
        //compute kernel coefficients
	sum = 0.0;
        for (i=0; i < A->n; i++)
        {
            d    = A->dx[i]*A->dx[i]+A->dy[i]*A->dy[i];
            w[i] = K2 * exp(-d/K1); // Gaussian
	    sum += w[i];
        }

        // Convolution

        for (p=0; p < f->nelems; p++)
        {
            u.x = p%f->ncols;
            u.y = p/f->ncols;
            val = 0.0;
            for (i=0; i < A->n; i++)
            {
                v.x = u.x + A->dx[i];
                v.y = u.y + A->dy[i];
                if (ValidPixel(img,v.x,v.y))
                {
                    q   = v.x + img->tbrow[v.y];
                    val += (float)img->val[q]*w[i];
                }
            }
            f->elem[p].feat[s-1]=(int)val/(sum*f->Imax);
        }
        free(w);

        DestroyAdjRel(&A);
    }
    return(f);
}


Features *GaussCImageFeats(CImage *cimg, int nscales)
{
    Features *f=NULL;
    AdjRel   *A=NULL;
    int       s,i,j,p,q;
    Pixel     u,v;
    Image    *img1;
    float    *w,d,K,sigma2,val,sum;

    f = CreateFeatures(cimg->C[0]->ncols, cimg->C[0]->nrows, 3*nscales);

    f->Imax = MAX(MAX(MaximumValue(cimg->C[0]),MaximumValue(cimg->C[1])),MaximumValue(cimg->C[2]));

    for (j=0; j < 3; j=j+1)
    {
        img1 = cimg->C[j];
        for (s=1; s <= nscales; s=s+1)
        {
            A  = Circular(s);
            w  = AllocFloatArray(A->n);
            sigma2 = (s/3.0)*(s/3.0);
            K      =  2.0*sigma2;

            //compute kernel coefficients
	    sum = 0.0;
            for (i=0; i < A->n; i++)
            {
                d    = A->dx[i]*A->dx[i]+A->dy[i]*A->dy[i];
                w[i] = 1.0/sqrt(2.0*PI*sigma2) * exp(-d/K); // Gaussian
		sum += w[i];
	    }

            // Convolution

            for (p=0; p < f->nelems; p++)
            {
                u.x = p%f->ncols;
                u.y = p/f->ncols;
                val = 0.0;
                for (i=0; i < A->n; i++)
                {
                    v.x = u.x + A->dx[i];
                    v.y = u.y + A->dy[i];
                    if (ValidPixel(img1,v.x,v.y))
                    {
                        q   = v.x + img1->tbrow[v.y];
                        val += (float)img1->val[q]*w[i];
                    }
                }
                f->elem[p].feat[s-1+(j*nscales)] = val/(sum*f->Imax);
            }
            free(w);

            // Copy features and reinitialize images

            DestroyAdjRel(&A);
        }
    }
    return(f);
}


Features* LabFeats(Features* rgb)
{
    int nrows=rgb->nrows;
    int ncols=rgb->ncols;

    int tam = nrows * ncols;

    Features* f = CreateFeatures(ncols,nrows,rgb->nfeats);

    int z,i;
    float Imax = FLT_MIN;
    const int nscales = rgb->nfeats/3;

    // generalization for multiple scales
    // our assumption is that the feature vectors
    // in *rgb follow:
    // R_1,R_2,...,R_nscales-1,G_1,G_2,...,G_nscales-1,B_1,B_2,...,B_nscales-1
    for(i = 0; i < nscales; i++)
    {
        for (z = 0;z < tam;z++)
        {
            //******* RGB original******//
            float R = rgb->elem[z].feat[i];
            float G = rgb->elem[z].feat[nscales+i];
            float B = rgb->elem[z].feat[2*nscales+i];

            RGB2Lab(&R,&G,&B);

            f->elem[z].feat[i] = R;
            f->elem[z].feat[nscales+i] = G;
            f->elem[z].feat[2*nscales+i] = B;

            Imax = MAX(Imax,MAX(R,MAX(G,B)));
        }
    }
    f->Imax = (int)Imax;

    return f;
}
