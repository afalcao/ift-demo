/*
    Copyright (C) <2010> <Alexandre Xavier FalcÃ£o and Thiago Vallin Spina>

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

#include "ift.h"

/* Papers related to this program:


@inproceedings{Spina09a,
author = "T.V. Spina and J.A. Montoya-Zegarra and A.X. Falc{\~{a}}o and P.A.V. Miranda",
title = "Fast interactive segmentation of natural images using the Image Foresting Transform",
booktitle = "Proc. of the 16th Intl. Conf. on Digital Signal Processing",
address = "Santorini, Greece",
publisher = "IEEE",
doi = "10.1109/ICDSP.2009.5201044",
year = 2009
}

@article{Papa09a,
author = "J.P. Papa and A.X. Falc{\~{a}}o and C.T.N. Suzuki",
title = "Supervised Pattern Classification based on Optimum-Path
Forest",
journal = "Intl. Journal of Imaging Systems and Technology",
publisher = "Wiley",
doi = "10.1002/ima.20188",
volume = "19",
number = "2",
pages = "120--131",
month = "Jun",
year = 2009
}

@article{Miranda10a,
author = "P.A.V. Miranda and A.X. Falc{\~{a}}o and J.K. Udupa",
title  = "Synergistic Arc-Weight Estimation for Interactive Image Segmentation using Graphs",
journal = "Computer Vision and Image Understanding",
publisher = "Elsevier",
doi = "10.1016/j.cviu.2009.08.001",
volume = 114,
number = 1,
pages = "85--99",
month = "Jan",
year = 2010
}

*/

#define MAXGRAD 255

/* Object map computation */

// Classification function: returns a fuzzy image with pixel values in [0,1] (where the
// higher the value the closer the similarity to the object)
DImage *FuzzyOPFClassifyImage(Subgraph *sgtrain, Features *feat)
{
  int i, p, k, c;
  float tmp, weight, minCost;
  double objcost, bkgcost, cost[2];// cost has cardinality 2 because it holds the minimum costs for the object and background classes
  DImage *result = CreateDImage(feat->ncols, feat->nrows);

  for(i = 0; i < feat->nelems; i++)
    {
      for(c = 1; c <= 2; c++)
	{
	  p = 0;
	  minCost = FLT_MAX;
	  do
	    {
	      k = sgtrain->ordered_list_of_nodes[p];
	      
	      if(sgtrain->node[k].label == c)
		{
		  // optimization which doesn't evaluate all the graph nodes when computing the optimum path
		  if(minCost <= sgtrain->node[k].pathval) break;

		  weight = EuclDistLog(feat->elem[i].feat, sgtrain->node[k].feat, sgtrain->nfeats);
		  tmp  = MAX(sgtrain->node[k].pathval, weight);

		  minCost = MIN(minCost,tmp);
		}
	      p++;
	    }
	  while(p < sgtrain->nnodes);

	  cost[c-1] = minCost;
	}

      bkgcost = cost[0];
      objcost = cost[1];

      if((objcost + bkgcost) < 0.00001 || objcost == bkgcost)
	{
	  result->val[i] = 0.5;
	}
      else
	{
	  result->val[i] = bkgcost / (bkgcost + objcost);
	}

    }
  return result;
}

/* Gradient computation */

// Object-based gradient computation
Image *ObjectGradient(DImage *img, float radius)
{
	real    dist, gx, gy;
	int     i, p, q, n = img->ncols * img->nrows;
	Pixel   u, v;
	AdjRel *A = Circular(radius);
	real   *md = AllocRealArray(A->n);

	Image *grad = CreateImage(img->ncols, img->nrows);

	double Imax = MaximumDImageValue(img);

	for(i = 1; i < A->n; i++)
		md[i] = sqrt(A->dx[i] * A->dx[i] + A->dy[i] * A->dy[i]);

	for(p = 0; p < n; p++)
	{
		u.x = p % img->ncols;
		u.y = p / img->ncols;

		gx = gy = 0.0;

		for(i = 1; i < A->n; i++)
		{
			v.x = u.x + A->dx[i];
			v.y = u.y + A->dy[i];
			if(ValidDImagePixel(img, v.x, v.y))
			{
				q    = v.x + img->tbrow[v.y];
				dist = ((float)img->val[q] - (float)img->val[p]) / Imax;

				gx  += dist * A->dx[i] / md[i];
				gy  += dist * A->dy[i] / md[i];
			}
		}
		grad->val[p] = MAXGRAD * sqrt(gx * gx + gy * gy);
	}

	free(md);
	DestroyAdjRel(&A);

	return(grad);
}

// Image-based gradient computation
Image *FeaturesGradient(Features *f, float radius)
{
	real    dist, gx, gy, mag;
	int     j, i, p, q, n = f->ncols * f->nrows;
	Pixel   u, v;
	AdjRel *A = Circular(radius);
	real   *md = AllocRealArray(A->n);

	Image *grad = CreateImage(f->ncols, f->nrows);

	for(i = 1; i < A->n; i++)
		md[i] = sqrt(A->dx[i] * A->dx[i] + A->dy[i] * A->dy[i]);

	for(p = 0; p < n; p++)
	{
		u.x = p % f->ncols;
		u.y = p / f->ncols;

		float max_mag = FLT_MIN;
		for(j = 0; j < f->nfeats; j++)
		{
			gx = gy = 0.0;
			for(i = 1; i < A->n; i++)
			{
				v.x = u.x + A->dx[i];
				v.y = u.y + A->dy[i];
				if((v.x >= 0 && v.x < f->ncols) && (v.y >= 0 && v.y < f->nrows))
				{
					q    = v.x + v.y * f->ncols;
					dist = (f->elem[q].feat[j] - f->elem[p].feat[j]);
					gx  += dist * A->dx[i] / md[i];
					gy  += dist * A->dy[i] / md[i];
				}
			}
			mag = sqrt(gx * gx + gy * gy);

			if(mag > max_mag)
				max_mag = mag;
		}
		grad->val[p] = (int)MAXGRAD * max_mag;
	}

	free(md);
	DestroyAdjRel(&A);

	return(grad);
}

// Linear combination of gradients
Image *CombineGradients(Image *objgrad, Image *imggrad, float wobj)
{
	int p;
	Image *grad = CreateImage(objgrad->ncols, objgrad->nrows);

	for(p = 0; p < objgrad->ncols * objgrad->nrows; p++)
		grad->val[p] = wobj * objgrad->val[p] + (1 - wobj) * imggrad->val[p];

	return grad;
}

int main(int argc, char **argv)
{
	float wobj = 0.5;
	char outfile[100];
	char *file_noext;
	timer    *t1 = NULL, *t2 = NULL;
	Image    *objgrad = NULL, *imggrad = NULL, *grad = NULL, *tmp = NULL;
	Features *feat = NULL;
	DImage   *objmap = NULL;
	Subgraph *sg = NULL, *sgtrainobj = NULL, *sgtrainbkg = NULL, *sgeval = NULL, *sgtrain = NULL;
	Set      *Obj = NULL, *Bkg = NULL;


	/* The following block must the remarked when using non-linux machines */

	void *trash = malloc(1);
	struct mallinfo info;
	int MemDinInicial, MemDinFinal;
	free(trash);
	info = mallinfo();
	MemDinInicial = info.uordblks;

	/*----------------------------------------------------------------------*/

	if(argc != 4)
	{
		fprintf(stderr, "Usage: %s <image.pgm (.ppm)>  <seeds.txt> <wobj [0,1]>\n", argv[0]);
		fprintf(stderr, "image.pgm (.ppm): image to be classified\n");
		fprintf(stderr, "seeds.txt: seed pixels\n");
		fprintf(stderr, "wobj: object gradient weight\n");
		exit(-1);
	}

	char *ext = strrchr(argv[1], '.');

	if(!strcmp(ext, ".pgm"))
	{
		Image   *img = NULL;
		img   = ReadImage(argv[1]);
		feat  = GaussImageFeats(img, 2);

		DestroyImage(&img);
	}
	else
	{
		CImage   *cimg = NULL;
		cimg   = ReadCImage(argv[1]);

		Features *gaussfeats   = GaussCImageFeats(cimg, 2);

		int p, j;
		//converting features from [0,1] to [0,255]
		for(p = 0; p < gaussfeats->nelems; p++)
			for(j = 0; j < gaussfeats->nfeats; j++)
				gaussfeats->elem[p].feat[j] *= gaussfeats->Imax;

		feat = LabFeats(gaussfeats);
		//converting features from [0,255] to [0,1]
		for(p = 0; p < feat->nelems; p++)
			for(j = 0; j < feat->nfeats; j++)
				feat->elem[p].feat[j] /= feat->Imax;

		DestroyCImage(&cimg);
		DestroyFeatures(&gaussfeats);
	}

	file_noext = strtok(argv[1], ".");

	ReadSeeds(argv[2], &Obj, &Bkg);

	wobj = atof(argv[3]);

	t1 = Tic();

	sg = SubgraphFromSeeds(feat, Obj, Bkg);
    SplitSubgraph(sg, &sgtrain, &sgeval, 0.2);

	/* supervised fuzzy classification using the Optimum-Path Forest classifier */
	OPFLearning(&sgtrain, &sgeval);
	objmap = FuzzyOPFClassifyImage(sgtrain, feat);

	/* computing object gradient */
	objgrad = ObjectGradient(objmap, 1.5);

	/* computing feature vector gradient */
	imggrad = FeaturesGradient(feat, 1.5);

	/* combining gradients */
	grad = CombineGradients(objgrad, imggrad, wobj);

	t2 = Toc();

	fprintf(stdout, "Gradient computing in %f ms\n", CTime(t1, t2));

	tmp = ConvertDImage2Image(objmap);
	sprintf(outfile, "%s_objmap.pgm", file_noext);
	WriteImage(tmp, outfile);
	DestroyImage(&tmp);

	sprintf(outfile, "%s_objgrad.pgm", file_noext);
	WriteImage(objgrad, outfile);
	sprintf(outfile, "%s_imggrad.pgm", file_noext);
	WriteImage(imggrad, outfile);
	sprintf(outfile, "%s_grad.pgm", file_noext);
	WriteImage(grad, outfile);

	DestroyDImage(&objmap);
	DestroyImage(&imggrad);
	DestroyImage(&objgrad);
	DestroyImage(&grad);
	DestroyFeatures(&feat);
	DestroySubgraph(&sg);
	DestroySubgraph(&sgtrain);
	DestroySubgraph(&sgtrainobj);
	DestroySubgraph(&sgtrainbkg);
	DestroySubgraph(&sgeval);
	DestroySet(&Obj);
	DestroySet(&Bkg);

	/* The following block must the remarked when using non-linux machines */

	info = mallinfo();
	MemDinFinal = info.uordblks;
	if(MemDinInicial != MemDinFinal)
		printf("\n\nDinamic memory was not completely deallocated (%d, %d)\n",
					 MemDinInicial, MemDinFinal);


	return(0);
}
