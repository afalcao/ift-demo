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

#include "subgraph.h"

/*----------- Constructor and destructor ------------------------*/
// Allocate nodes without features

Subgraph *CreateSubgraph(int nnodes)
{
    Subgraph *sg=(Subgraph *)calloc(1,sizeof(Subgraph));
    int i;

    sg->nnodes = nnodes;
    sg->node   = (SNode *)calloc(nnodes,sizeof(SNode));
    sg->ordered_list_of_nodes = (int *) calloc(nnodes,sizeof(int));

    if (sg->node == NULL)
    {
        Error("Cannot allocate nodes","CreateSubgraph");
    }

    for (i=0; i < sg->nnodes; i++)
    {
        sg->node[i].feat   = NULL;
        sg->node[i].adj    = NULL;
    }

    return(sg);
}

// Deallocate memory for subgraph
void DestroySubgraph(Subgraph **sg)
{
    int i;

    if ((*sg)!=NULL)
    {
        for (i=0; i < (*sg)->nnodes; i++)
        {
            if ((*sg)->node[i].feat != NULL)
                free((*sg)->node[i].feat);
            if ((*sg)->node[i].adj != NULL)
                DestroySet(&(*sg)->node[i].adj);
        }
        free((*sg)->node);
        free((*sg)->ordered_list_of_nodes);
        free((*sg));
        *sg = NULL;
    }
}

//write subgraph to disk
void WriteSubgraph(Subgraph *g, char *file){
  FILE *fp = NULL;
  int i, j;
  size_t result;

  if(g != NULL){
    fp = fopen(file, "wb");
    result = fwrite(&g->nnodes, sizeof(int), 1, fp);
    result = fwrite(&g->nlabels, sizeof(int), 1, fp);
    result = fwrite(&g->nfeats, sizeof(int), 1, fp);

    /*writing position(id), label and features*/
    for (i = 0; i < g->nnodes; i++){
      result = fwrite(&g->node[i].position, sizeof(int), 1, fp);
      result = fwrite(&g->node[i].truelabel, sizeof(int), 1, fp);
      for (j = 0; j < g->nfeats; j++)
	result = fwrite(&g->node[i].feat[j], sizeof(float), 1, fp);
    }
    fclose(fp);
  }
}

//read subgraph from opf format file
Subgraph *ReadSubgraph(char *file){
  Subgraph *g = NULL;
  FILE *fp = NULL;
  int nnodes, i, j;
  char msg[256];
  size_t result;

  if((fp = fopen(file, "rb")) == NULL){
    sprintf(msg, "%s%s", "Unable to open file ", file);
    Error(msg,"ReadSubGraph");
  }

  /*reading # of nodes, classes and feats*/
  result = fread(&nnodes, sizeof(int), 1, fp);
  g = CreateSubgraph(nnodes);
  result = fread(&g->nlabels, sizeof(int), 1, fp);
  result = fread(&g->nfeats, sizeof(int), 1, fp);

  /*reading features*/
  for (i = 0; i < g->nnodes; i++){
    g->node[i].feat = AllocFloatArray(g->nfeats);
    result = fread(&g->node[i].position, sizeof(int), 1, fp);
    result = fread(&g->node[i].truelabel, sizeof(int), 1, fp);
    for (j = 0; j < g->nfeats; j++)
      result = fread(&g->node[i].feat[j], sizeof(float), 1, fp);
  }

  fclose(fp);

  return g;
}

// Copy subgraph (also copy Arcs)
Subgraph *CopySubgraph(Subgraph *g){
  Subgraph *clone = NULL;
  int i;

  if(g != NULL){
    clone = CreateSubgraph(g->nnodes);

	clone->bestk = g->bestk;
	clone->df = g->df;
	clone->nlabels = g->nlabels;
	clone->nfeats = g->nfeats;
	clone->mindens = g->mindens;
	clone->maxdens = g->maxdens;
	clone->K = g->K;
	clone->Imax = g->Imax;

    for(i=0; i< g->nnodes; i++){
      CopySNode(&clone->node[i], &g->node[i], g->nfeats);
      clone->ordered_list_of_nodes[i] = g->ordered_list_of_nodes[i];
    }

    return clone;
  }else return NULL;
}

//Copy nodes
void CopySNode(SNode *dest, SNode *src, int nfeats){
  dest->feat = AllocFloatArray(nfeats);
  memcpy(dest->feat, src->feat, nfeats*sizeof(float));
  dest->pathval = src->pathval;
  dest->dens = src->dens;
  dest->label  = src->label;
  dest->root = src->root;
  dest->pred  = src->pred;
  dest->truelabel = src->truelabel;
  dest->position = src->position;
  dest->pixel = src->pixel;
  dest->status = src->status;
  dest->relevant = src->relevant;
  dest->radius = src->radius;
  dest->nplatadj = src->nplatadj;

  dest->adj = CloneSet(src->adj);
}

//Swap nodes
void SwapSNode(SNode *a, SNode *b){
	SNode tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

