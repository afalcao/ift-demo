/*
  Copyright (C) <2009> <Alexandre Xavier Falcão and João Paulo Papa>

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

  please see full copyright in COPYING file.
  -------------------------------------------------------------------------
  written by A.X. Falcão <afalcao@ic.unicamp.br> and by J.P. Papa
  <papa.joaopaulo@gmail.com>, Oct 20th 2008

  This program is a collection of functions to manage the Optimum-Path Forest (OPF)
  classifier.*/

#include "opf.h"

/* Functions to create Subgraphs for OPF */

void SetSubgraphFeatures(Subgraph *sg, Features *f)
{
    int i,j;

    sg->nfeats = f->nfeats;
    for (i=0; i < sg->nnodes; i++)
    {
        sg->node[i].feat = AllocFloatArray(sg->nfeats);

        for (j=0; j < sg->nfeats; j++)
        {
            sg->node[i].feat[j] = f->elem[sg->node[i].pixel].feat[j];
        }
    }
    sg->Imax=f->Imax;
}

Subgraph* SubgraphFromSeeds(Features* f, Set* Si, Set* Se)
{

    Subgraph *sg=NULL;
    Set *aux;
    int ni,ne,i;

    ni = GetSetSize(Si);
    ne = GetSetSize(Se);

    if (ni == 0 || ne == 0) return NULL;

    ///--------CreateSubGraph-----------------

    sg = CreateSubgraph(ni+ne);
    sg->nlabels = 2;

    if (sg->node == NULL) Error((char*)"Cannot allocate nodes",(char*)"usis_subgraph_from_markers");

    i = 0;
    aux = Si;
    while (aux != NULL)
    {
        sg->node[i].pixel  = aux->elem;
        sg->node[i].position  = aux->elem;
        sg->node[i].truelabel = OPF_OBJ_LABEL; // using 2 for object because OPF labels start at 1
        sg->node[i].status=0; // training node
        aux = aux->next;
        i++;
    }
    aux = Se;
    while (aux != NULL)
    {
        sg->node[i].pixel  = aux->elem;
        sg->node[i].position  = aux->elem;
        sg->node[i].truelabel = OPF_BKG_LABEL; // using 1 for background because OPF labels start at 1
        sg->node[i].status=0; // training node
        aux = aux->next;
        i++;
    }

    SetSubgraphFeatures(sg,f);

    return sg;
}

// Split subgraph into two parts such that the size of the first part
// is given by a percentual of samples.
void SplitSubgraph(Subgraph *sg, Subgraph **sg1, Subgraph **sg2, float perc1)
{
    int *label=AllocIntArray(sg->nlabels+1),i,j,i1,i2;
    int *nelems=AllocIntArray(sg->nlabels+1),totelems;
    srandom((int)time(NULL));

    for (i=0; i < sg->nnodes; i++)
    {
        sg->node[i].status = 0;
        label[sg->node[i].truelabel]++;
    }

    for (i=0; i < sg->nnodes; i++)
    {
        nelems[sg->node[i].truelabel]=MAX((int)(perc1*label[sg->node[i].truelabel]),1);
    }

    free(label);

    totelems=0;
    for (j=1; j <= sg->nlabels; j++)
        totelems += nelems[j];

    *sg1 = CreateSubgraph(totelems);
    *sg2 = CreateSubgraph(sg->nnodes-totelems);
    (*sg1)->nfeats = sg->nfeats;
    (*sg2)->nfeats = sg->nfeats;

    for (i1=0; i1 < (*sg1)->nnodes; i1++)
        (*sg1)->node[i1].feat = AllocFloatArray((*sg1)->nfeats);
    for (i2=0; i2 < (*sg2)->nnodes; i2++)
        (*sg2)->node[i2].feat = AllocFloatArray((*sg2)->nfeats);

    (*sg1)->nlabels = sg->nlabels;
    (*sg2)->nlabels = sg->nlabels;

    i1=0;
    while (totelems > 0)
    {
        i = RandomInteger(0,sg->nnodes-1);
        if (sg->node[i].status!=NIL)
        {
            if (nelems[sg->node[i].truelabel]>0) // copy node to sg1
            {
                (*sg1)->node[i1].position = sg->node[i].position;
                for (j=0; j < (*sg1)->nfeats; j++)
                    (*sg1)->node[i1].feat[j]=sg->node[i].feat[j];
                (*sg1)->node[i1].truelabel = sg->node[i].truelabel;
                i1++;
                nelems[sg->node[i].truelabel] = nelems[sg->node[i].truelabel] - 1;
                sg->node[i].status = NIL;
                totelems--;
            }
        }
    }

    i2=0;
    for (i=0; i < sg->nnodes; i++)
    {
        if (sg->node[i].status!=NIL)
        {
            (*sg2)->node[i2].position = sg->node[i].position;
            for (j=0; j < (*sg2)->nfeats; j++)
                (*sg2)->node[i2].feat[j]=sg->node[i].feat[j];
            (*sg2)->node[i2].truelabel = sg->node[i].truelabel;
            i2++;
        }
    }

    free(nelems);
}


Subgraph* SplitSubgraphByTrueLabel(Subgraph* sg, int label)
{
    int nnodes = 0;
    int i, i0 = 0;

    for(i = 0; i < sg->nnodes; i++)
        if(sg->node[i].truelabel == label) nnodes++;

    Subgraph* sgresult = CreateSubgraph(nnodes);

    sgresult->nfeats = sg->nfeats;
    sgresult->df = sg->df;

    for(i = 0; i < sg->nnodes; i++)
    {
        if(sg->node[i].truelabel == label)
        {
            CopySNode(&sgresult->node[i0++], &sg->node[i], sg->nfeats);
        }
    }

    return sgresult;
}

/* OPF-related functions */

// Compute Euclidean distance between feature vectors
float EuclDist(float *f1, float *f2, int n)
{
    int i;
    float dist=0.0f;

    for (i=0; i < n; i++)
        dist += (f1[i]-f2[i])*(f1[i]-f2[i]);

    return(dist);
}


// Discretizes original distance
float EuclDistLog(float *f1, float *f2, int n)
{
    return(((float)MAXARCW*log(EuclDist(f1,f2,n)+1)));
}


// Compute accuracy
float Accuracy(Subgraph *sg)
{
    float Acc = 0.0f, **error_matrix = NULL, error = 0.0f;
    int i, *nclass = NULL, nlabels=0;

    error_matrix = (float **)calloc(sg->nlabels+1, sizeof(float *));
    for (i=0; i<= sg->nlabels; i++)
        error_matrix[i] = (float *)calloc(2, sizeof(float));

    nclass = AllocIntArray(sg->nlabels+1);

    for (i = 0; i < sg->nnodes; i++)
    {
        nclass[sg->node[i].truelabel]++;
    }

    for (i = 0; i < sg->nnodes; i++)
    {
        if (sg->node[i].truelabel != sg->node[i].label)
        {
            error_matrix[sg->node[i].truelabel][1]++;
            error_matrix[sg->node[i].label][0]++;
        }
    }

    for (i=1; i <= sg->nlabels; i++)
    {
        if (nclass[i]!=0)
        {
            error_matrix[i][1] /= (float)nclass[i];
            error_matrix[i][0] /= (float)(sg->nnodes - nclass[i]);
            nlabels++;
        }
    }

    for (i=1; i <= sg->nlabels; i++)
    {
        if (nclass[i]!=0)
            error += (error_matrix[i][0]+error_matrix[i][1]);
    }

    Acc = 1.0-(error/(2.0*nlabels));

    for (i=0; i <= sg->nlabels; i++)
        free(error_matrix[i]);
    free(error_matrix);
    free(nclass);

    return(Acc);
}


//Replace errors from evaluating set by non prototypes from training set
void SwapErrorsbyNonPrototypes(Subgraph **sgtrain, Subgraph **sgeval)
{
    int i, j, counter, nonprototypes = 0, nerrors = 0;

    for (i = 0; i < (*sgtrain)->nnodes; i++)
    {
        if ((*sgtrain)->node[i].pred != NIL) // non prototype
        {
            nonprototypes++;
        }
    }

    for (i = 0; i < (*sgeval)->nnodes; i++)
        if ((*sgeval)->node[i].label != (*sgeval)->node[i].truelabel) nerrors++;

    for (i = 0; i < (*sgeval)->nnodes && nonprototypes >0 && nerrors > 0; i++)
    {
        if ((*sgeval)->node[i].label != (*sgeval)->node[i].truelabel)
        {
            counter = nonprototypes;
            while (counter > 0)
            {
                j = RandomInteger(0,(*sgtrain)->nnodes-1);
                if ((*sgtrain)->node[j].pred!=NIL)
                {
                    SwapSNode(&((*sgtrain)->node[j]), &((*sgeval)->node[i]));
                    (*sgtrain)->node[j].pred = NIL;
                    nonprototypes--;
                    nerrors--;
                    counter = 0;
                }
                else counter--;
            }
        }
    }
}


// Find prototypes by the MST approach
void MSTPrototypes(Subgraph *sg)
{
    int p,q;
    float weight;
    RealHeap *Q=NULL;
    float *pathval = NULL;
    int  pred;
    float nproto;

    // initialization
    pathval = AllocFloatArray(sg->nnodes);
    Q = CreateRealHeap(sg->nnodes, pathval);

    for (p = 0; p < sg->nnodes; p++)
    {
        pathval[ p ] = FLT_MAX;
        sg->node[p].status=0;
    }

    pathval[0]  = 0;
    sg->node[0].pred = NIL;
    InsertRealHeap(Q, 0);

    nproto=0.0;

    // Prim's algorithm for Minimum Spanning Tree
    while ( !IsEmptyRealHeap(Q) )
    {
        RemoveRealHeap(Q,&p);
        sg->node[p].pathval = pathval[p];

        pred=sg->node[p].pred;
        if (pred!=NIL)
            if (sg->node[p].truelabel != sg->node[pred].truelabel)
            {
                if (sg->node[p].status!=PROTOTYPE)
                {
                    sg->node[p].status=PROTOTYPE;
                    nproto++;
                }
                if (sg->node[pred].status!=PROTOTYPE)
                {
                    sg->node[pred].status=PROTOTYPE;
                    nproto++;
                }
            }

        for (q=0; q < sg->nnodes; q++)
        {
            if (Q->color[q]!=BLACK)
            {
                if (p!=q)
                {
                    weight = EuclDistLog(sg->node[p].feat,sg->node[q].feat,sg->nfeats);
                    if ( weight < pathval[ q ] )
                    {
                        sg->node[q].pred = p;
                        UpdateRealHeap(Q, q, weight);
                    }
                }
            }
        }
    }
    DestroyRealHeap(&Q);
    free( pathval );

}

void OPFTraining(Subgraph *sg)
{
    int p,q, i;
    float tmp,weight;
    RealHeap *Q = NULL;
    float *pathval = NULL;

    // compute optimum prototypes
    MSTPrototypes(sg);

    // initialization
    pathval = AllocFloatArray(sg->nnodes);

    Q=CreateRealHeap(sg->nnodes, pathval);

    for (p = 0; p < sg->nnodes; p++)
    {
        if (sg->node[p].status==PROTOTYPE)
        {
            sg->node[p].pred   = NIL;
            pathval[p]         = 0;
            sg->node[p].label  = sg->node[p].truelabel;
            InsertRealHeap(Q, p);
        }
        else  // non-prototypes
        {
            pathval[p]  = FLT_MAX;
        }
    }

    // IFT with fmax
    i=0;
    while ( !IsEmptyRealHeap(Q) )
    {
        RemoveRealHeap(Q,&p);

        sg->ordered_list_of_nodes[i]=p;
        i++;
        sg->node[p].pathval = pathval[p];

        for (q=0; q < sg->nnodes; q++)
        {
            if (p!=q)
            {
                if (pathval[p] < pathval[q])
                {
		    weight = EuclDistLog(sg->node[p].feat,sg->node[q].feat,sg->nfeats);

                    tmp  = MAX(pathval[p],weight);
                    if ( tmp < pathval[ q ] )
                    {
                        sg->node[q].pred  = p;
                        sg->node[q].label = sg->node[p].label;
                        UpdateRealHeap(Q, q, tmp);
                    }
                }
            }
        }
    }

    DestroyRealHeap( &Q );
    free( pathval );
}

//Classification function: it simply classifies samples from sg -----
void OPFClassify(Subgraph *sgtrain, Subgraph *sg)
{
    int i, j, k, l, label = -1;
    float tmp, weight, minCost;

    for (i = 0; i < sg->nnodes; i++)
    {
        j       = 0;
        k       = sgtrain->ordered_list_of_nodes[j];
        weight = EuclDistLog(sgtrain->node[k].feat,sg->node[i].feat,sg->nfeats);

        minCost = MAX(sgtrain->node[k].pathval, weight);
        label   = sgtrain->node[k].label;

        while ((j < sgtrain->nnodes-1)&&
                (minCost > sgtrain->node[sgtrain->ordered_list_of_nodes[j+1]].pathval))
        {

            l  = sgtrain->ordered_list_of_nodes[j+1];

            weight = EuclDistLog(sgtrain->node[l].feat,sg->node[i].feat,sg->nfeats);

            tmp = MAX(sgtrain->node[l].pathval, weight);
            if (tmp < minCost)
            {
                minCost = tmp;
                label = sgtrain->node[l].label;
            }
            j++;
            k  = l;
        }
        sg->node[i].label = label;
    }
}

//Learning function: it executes the learning procedure for CompGraph replacing the
//missclassified samples in the evaluation set by non prototypes from
//training set -----
void OPFLearning(Subgraph **sgtrain, Subgraph **sgeval)
{
    int i = 0, iterations = 10;
    float Acc = FLT_MIN, AccAnt = FLT_MIN,MaxAcc=FLT_MIN, delta;
    Subgraph *sg=NULL;

    do
    {
        AccAnt = Acc;
        fflush(stdout);
        fprintf(stdout, "\nrunning iteration ... %d ", i);
        OPFTraining(*sgtrain);
        OPFClassify(*sgtrain, *sgeval);
        Acc = Accuracy(*sgeval);
        if (Acc > MaxAcc)
        {
            MaxAcc = Acc;
            if (sg!=NULL) DestroySubgraph(&sg);
            sg = CopySubgraph(*sgtrain);
        }
        SwapErrorsbyNonPrototypes(&(*sgtrain), &(*sgeval));
        fflush(stdout);
        fprintf(stdout,"Accuracy in the evaluation set: %.2f %%\n", Acc*100);
        i++;
        delta = fabs(Acc-AccAnt);
    }
    while ((delta > 0.0001) && (i <= iterations));
    DestroySubgraph(&(*sgtrain));
    *sgtrain = sg;
}

//Classification function: returns a binary image where object pixels have value 1 and background pixels value 0
Image* OPFClassifyImage(Subgraph *sgtrain, Features* feat)
{
    int i, j, k, l, label = -1;
    float tmp, weight, minCost;
    Image* result = CreateImage(feat->ncols, feat->nrows);

    for (i = 0; i < feat->nelems; i++)
    {
        j       = 0;
        k       = sgtrain->ordered_list_of_nodes[j];

	weight = EuclDistLog(sgtrain->node[k].feat,feat->elem[i].feat,sgtrain->nfeats);

        minCost = MAX(sgtrain->node[k].pathval, weight);
        label   = sgtrain->node[k].label;

        while ((j < sgtrain->nnodes-1)&&
                (minCost > sgtrain->node[sgtrain->ordered_list_of_nodes[j+1]].pathval))
        {
            l  = sgtrain->ordered_list_of_nodes[j+1];

            weight = EuclDistLog(sgtrain->node[l].feat,feat->elem[i].feat,sgtrain->nfeats);

	    tmp = MAX(sgtrain->node[l].pathval, weight);
            if (tmp < minCost)
            {
                minCost = tmp;
                label = sgtrain->node[l].label;
            }
            j++;
            k  = l;
        }
        result->val[i] = label - 1; //subtracting 1 from label so that
	// object pixels have value 1 whereas background pixels will
	// have label 0
    }

    return result;
}
