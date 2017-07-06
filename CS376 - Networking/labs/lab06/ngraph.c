

			  /* this program prompts the user to enter the
			name of a network graph data file (see the
			sample "ngraph.dat") and and does some analysis 
			of the graph  
							Marc Thomas
									*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXNODES 20
#define INFTY 32000
#define UNSCANNED -20


char	infile[128];
FILE	*fp;

	/* global graph information */

struct	nodeinfo {
	char	name[32];
	int	type;                   /* unused right now        */
	int	bufs;                   /* unused right now        */
	}  nodes[ MAXNODES ];
int	numnodes;                       /* number of nodes read in */
int	metric[ MAXNODES ][ MAXNODES ];           /* matric matrix */
int	capacity[ MAXNODES ][ MAXNODES ];       /* capacity matrix */
int	kmat[ MAXNODES ][ MAXNODES ];

	/* variables global to max_flow(), walk(), layer_net()
	   find_ref(), and push_pull()  */

int	fdest,fsrc,fdisp,fminpot,frefnode;
int	flayer[ MAXNODES ];
int	fcap[ MAXNODES ][ MAXNODES ],flow[ MAXNODES ][ MAXNODES ];

	/* static variables needed by main() */

int	spath[ MAXNODES ];
int	pathlen;

			/* Function Prototypes */
#if !defined(_NOPROTO)
int print_mat(int mat[][ MAXNODES ]);
int short_path(int *pnum, int path[], int dest, int src,
		int mat[][ MAXNODES ], int df);
int min(int i, int j);
int walk(int node);
int layer_net(void);
int find_ref(int node);
int push_pull(int node, int flowleft, int way);
int max_flow(int d, int s, int mat[][ MAXNODES ], int df);

int dinit(void);
int dmargin(int top, int bottom);
int dclscrn(void);
int dcl2eos(void);
int dcl2eol(void);
int dmove(int row, int col);
int dhlon(void);
int dhloff(void);
int dbold(void);
int dnobold(void);
char dgetesc(void);
int dcrlf(void);
int dbell(void);
int dpush(void);
int dpop(void);

#else
int print_mat();
int short_path();
int min();
int walk();
int layer_net();
int find_ref();
int push_pull();
int max_flow();

int dinit();     
int dclscrn();
int dcl2eos();
int dcl2eol();
int dmove();
int dhlon();
int dhloff();
char dgetesc();
int dcrlf();
int dbell();
int dpush();
int dpop();

#endif

/* --------------------------------------------------------------- */

#if defined(_NOPROTO)
int main(argc, argv, envp)
int	argc;
char	*argv[], *envp[];
#else
int main(int argc, char *argv[], char *envp[])
#endif
	{
	int	i, j, k, l, d, s, f, ret;
	char	khar, chrline[132], scanline[80];

/* initialize data structures and screen */

	numnodes = 0;
	ret = dinit();
	ret = dclscrn();

/* get graph input file */

	printf("enter name of graph input file: ");
	gets(infile);

	fp = fopen(infile,"r");  /* open file for reading */
	if (fp == NULL)
		{
		ret = dbell();
		printf("no such file exists. \n");
		return(-1);
		}

/* read in network graph data from input file */

	fscanf(fp,"%d",&numnodes);                   /* number of nodes */

/* now can initialize nodes and matrices since numnodes is known */

	for (i = 0 ; i < numnodes ; i++)
		{
		for (j = 0 ; j < 31 ; j++)
		  nodes[i].name[j] = ' ';
		nodes[i].name[31] = '\0';
		nodes[i].type = 0;
		nodes[i].bufs = 0;
		}
	for (i = 0 ; i < numnodes ; i++)
		for (j = 0 ; j < numnodes ; j++)
		  {
		  if (i == j) metric[i][j] = 0;
		    else metric[i][j] = INFTY;
		  capacity[i][j] = 0;
		  kmat[i][j] = 0;
		  }

/* read in node names */

	for (i = 0 ; i < numnodes ; i++)
		{
		fscanf(fp,"%d %s",&j,chrline);
		if ((j >= 0) && (j < numnodes))
			strcpy(nodes[j].name,chrline);
		}

/* remove comment line */

	fscanf(fp,"%s",chrline);

/* read in adjacencies and weights */

	while (fscanf(fp,"%d %d %d %d",&i,&j,&k,&l) != EOF)
		 /* from   to   metric(cost,dist,etc.) capacity */
		{
		metric[i][j] = k;
		metric[j][i] = k;
		if (l > 0)
		  {
		  kmat[i][j] = 1;
		  kmat[j][i] = 1;
		  }
		capacity[i][j] = l;
		capacity[j][i] = l;
		}

rloop:	printf("<m> to show metric matrix, ");
	printf("<c> to show capacity matrix, ");
	printf("<s> for shortest path, \n");
	printf("<f> for maximum flow, ");
	printf("<k> for connectivity, <S> for shortest path table, \n");
	printf("<F> for maximum flow table, <K> for connectivity table, ");
	printf("or <q> to quit: ");
rlp1:	gets(chrline);
	if (chrline[0] == '\0') goto rlp1;
	printf("\n");

	if ((chrline[0] == 'm') || (chrline[0] == 'c') ||
	    (chrline[0] == 's') || (chrline[0] == 'f') ||
	    (chrline[0] == 'k') || (chrline[0] == 'q') ||
	    (chrline[0] == 'S') || (chrline[0] == 'F') ||
	    (chrline[0] == 'K'))
		khar = chrline[0];
	else
		goto rloop;

	switch(khar) {

	case 'm':   /* show metric matrix */
	  printf("metric matrix: \n");
	  print_mat(metric);
	  printf("\n");
	  goto rloop;

	case 'c':   /* show capacity matrix */
	  printf("capacity matrix: \n");
	  print_mat(capacity);
	  printf("\n");
	  goto rloop;

	case 's':   /* shortest path analysis, ask for dest., source */
rtry1:	  printf("enter destination node num ");
	  gets(scanline);	/* do some error checking for scanf,
				   which is a kid's gloves function */
	  i = 0;
	  while(scanline[i] != '\0')
		{
		if ((scanline[i] < 48) || (scanline[i] > 57))
			{
			printf("error: please enter a NUMBER \n");
			goto rtry1;
			}
		i++;
		}
	  scanline[i] = '\0';		/* terminate string */
	  sscanf(scanline,"%d",&d);	/* destination */

	  if ((d < 0) || (d >= numnodes)) goto rtry1;
rtry2:	  printf("enter source node num ");
	  gets(scanline);	/* do some error checking for scanf,
				   which is a kid's gloves function */
	  i = 0;
	  while(scanline[i] != '\0')
		{
		if ((scanline[i] < 48) || (scanline[i] > 57))
			{
			printf("error: please enter a NUMBER \n");
			goto rtry2;
			}
		i++;
		}
	  scanline[i] = '\0';		/* terminate string */
	  sscanf(scanline,"%d",&s);	/* source */

	  if ((s < 0) || (s >= numnodes)) goto rtry2;
	  if (s == d)
		{
		printf("source and destination are the same. \n");
		goto rtry1;
		}
	  printf("\n");

	  j = short_path(&pathlen,spath,d,s,metric,1); /* run shortest path
							algorithm */
	  printf("\n shortest path length is %d units as follows: \n",j);
	  printf("node number %d   ( name =  %5s ) \n",spath[0],
			nodes[spath[0]].name);
	  j = 0;
	  for (i = 1 ; i < pathlen ; i++)
		{
		j = j + metric[spath[i]][spath[i-1]];
		printf("node number %d   ( name =  %5s ) ",
			spath[i],nodes[spath[i]].name);
		printf("( dist = %5d ) \n",j); 
		}
	  printf("\n");
	  goto rloop;  /* end of case 's' */

	case 'f':      /* find maximum flow between two nodes */
rtry3:	  printf("enter destination node num ");
	  gets(scanline);	/* do some error checking for scanf,
				   which is a kid's gloves function */
	  i = 0;
	  while(scanline[i] != '\0')
		{
		if ((scanline[i] < 48) || (scanline[i] > 57))
			{
			printf("error: please enter a NUMBER \n");
			goto rtry3;
			}
		i++;
		}
	  scanline[i] = '\0';		/* terminate string */
	  sscanf(scanline,"%d",&d);	/* destination */

	  if ((d < 0) || (d >= numnodes)) goto rtry3;
rtry4:	  printf("enter source node num ");
	  gets(scanline);	/* do some error checking for scanf,
				   which is a kid's gloves function */
	  i = 0;
	  while(scanline[i] != '\0')
		{
		if ((scanline[i] < 48) || (scanline[i] > 57))
			{
			printf("error: please enter a NUMBER \n");
			goto rtry4;
			}
		i++;
		}
	  scanline[i] = '\0';		/* terminate string */
	  sscanf(scanline,"%d",&s);	/* source */

	  if ((s < 0) || (s >= numnodes)) goto rtry4;
	  if (s == d)
		{
		printf("source and destination are the same. \n");
		goto rtry3;
		}
	  printf("\n");

	  f = max_flow(d,s,capacity,1);  /* run max flow algorithm, get
		answer in f, complete info is left in flow[][] */

	  printf("\n");
	  printf("the maximum flow from node %d to node %d ",s,d);
	    printf("is %d units. flow matrix: \n",f);
	  printf("\n");
	  printf("      ");
	  for (j = 0 ; j < numnodes ; j++) printf(" %5s ",nodes[j].name);
	  printf("\n");
	  for (i = 0 ; i < numnodes ; i++)
		{
		printf("%5s ",nodes[i].name);
		for (j = 0 ; j < numnodes ; j++)
		  {
		  k = flow[i][j];
		  if ((k > 0) && (k == capacity[i][j])) ret = dhlon();
		  printf(" %5d ",k);
		  if ((k > 0) && (k == capacity[i][j])) ret = dhloff();
		  }
		printf("\n");
		}
	  printf("\n");
	  goto rloop;  /* end of case 'f' */

	case 'k':      /* find  arc-connectivity between nodes */
rtry5:	  printf("enter first node num ");
	  gets(scanline);	/* do some error checking for scanf,
				   which is a kid's gloves function */
	  i = 0;
	  while(scanline[i] != '\0')
		{
		if ((scanline[i] < 48) || (scanline[i] > 57))
			{
			printf("error: please enter a NUMBER \n");
			goto rtry5;
			}
		i++;
		}
	  scanline[i] = '\0';		/* terminate string */
	  sscanf(scanline,"%d",&d);	/* destination */

	  if ((d < 0) || (d >= numnodes)) goto rtry5;
rtry6:	  printf("enter second node num ");
	  gets(scanline);	/* do some error checking for scanf,
				   which is a kid's gloves function */
	  i = 0;
	  while(scanline[i] != '\0')
		{
		if ((scanline[i] < 48) || (scanline[i] > 57))
			{
			printf("error: please enter a NUMBER \n");
			goto rtry6;
			}
		i++;
		}
	  scanline[i] = '\0';		/* terminate string */
	  sscanf(scanline,"%d",&s);	/* source */

	  if ((s < 0) || (s >= numnodes)) goto rtry6;
	  if (s == d)
		{
		printf("nodes are the same. \n");
		goto rtry5;
		}
	  printf("\n");

	  f = max_flow(d,s,kmat,1);   /* run max flow algorithm, but
		with kmat[][] rather than capacity[][]  */

	  printf("\n");
	  printf("the arc-connectivity from node %d to node %d ",s,d);
	    printf("is %d-fold.\n",f);
	  printf("\n");
	  goto rloop;  /* end of case 'k' */

	case 'S':	/* construct table of shortest paths */
	  printf("\n");
	  printf("shortest path table: \n\n");
	  printf("       ");
	  for (j = 0 ; j < numnodes ; j++) printf(" %5s ",nodes[j].name);
	  printf("\n");
	  for (i = 0 ; i < numnodes ; i++)
	    {
	    printf("%5s ",nodes[i].name);
	    for (j = 0 ; j < i ; j++)
	      {
	      printf(" %5d ",short_path(&pathlen,spath,i,j,metric,0));
	      }
	    printf("\n");
	    }
	  printf("\n");
	  goto rloop;  /* end of case 'S' */

	case 'F':	/* construct table of maximum flosw */
	  printf("\n");
	  printf("maximum flow table: \n\n");
	  printf("       ");
	  for (j = 0 ; j < numnodes ; j++) printf(" %5s ",nodes[j].name);
	  printf("\n");
	  for (i = 0 ; i < numnodes ; i++)
	    {
	    printf("%5s ",nodes[i].name);
	    for (j = 0 ; j < i ; j++)
	      {
	      printf(" %5d ",max_flow(i,j,capacity,0));
	      }
	    printf("\n");
	    }
	  printf("\n");
	  goto rloop;  /* end of case 'F' */

	case 'K':	/* construct table arc connectivities */
	  printf("\n");
	  printf("arc connectivities: \n\n");
	  printf("       ");
	  for (j = 0 ; j < numnodes ; j++) printf(" %5s ",nodes[j].name);
	  printf("\n");
	  for (i = 0 ; i < numnodes ; i++)
	    {
	    printf("%5s ",nodes[i].name);
	    for (j = 0 ; j < i ; j++)
	      {
	      printf(" %5d ",max_flow(i,j,kmat,0));
	      }
	    printf("\n");
	    }
	  printf("\n");
	  goto rloop;  /* end of case 'K' */

	case 'q':      /* close files and quit program */
	  goto finish;

	}  /* end of switch */

finish: 
	fclose(fp); /* close input graph file */
	return(0);
	}  /* end of main */

/* ----------------------------------------------------------------
Graph Algorithm Routines:
   ---------------------------------------------------------------- */

#if defined(_NOPROTO)
print_mat(mat) 
int	mat[][ MAXNODES ];
#else
int print_mat(int mat[][ MAXNODES ])
#endif
             /* prints metric or capacity matrix on the screen */
	{
	int	i,j;

	printf("\n");
	printf("number of nodes is %d and matrix is: \n\n",numnodes);
	printf("      ");
	for (j = 0 ; j < numnodes ; j++) printf(" %5s ",nodes[j].name);
	printf("\n");
	for (i = 0 ; i < numnodes ; i++)
		{
		printf("%5s ",nodes[i].name);
		for (j = 0 ; j < numnodes ; j++)
		  if (mat[i][j] == INFTY) printf("       ");
		    else printf(" %5d ",mat[i][j]);
		printf("\n");
		}
	printf("\n");
	return(0);
	}  /* end of print_mat */

/* ----------------------------------------------------------- */

#if defined(_NOPROTO)
short_path(pnum,path,dest,src,mat,df)  
int	dest,src,df;
int	*pnum,path[],mat[][ MAXNODES ];
#else
int short_path(int *pnum, int path[], int dest, int src,
		int mat[][ MAXNODES ], int df)
#endif
				/* Dijkstra's shortest path
			algorithm for a path from scr to dest. If
			df (display flag) is set then progress of
			algorithm is shown on screen. Path itself
			is in *pnum,path[] and function returns
			total distance.  */
	{
	int	i,k,m,j,sum,ret;
	struct {
	  int	perm;
	  int	predecess;
	  int	len;} nodestate[ MAXNODES ];

	for (i = 0 ; i < numnodes ; i++)    /* initialize */
	  {
	  nodestate[i].perm = 0;
	  nodestate[i].predecess = -1;
	  nodestate[i].len = INFTY;
	  }

	nodestate[dest].perm = 1;      /* start at destination */
	nodestate[dest].len = 0;       /* no accumulated length */
	k = dest;                      /* k is initial working node */
	if (df == 1)		    /* display on screen if df == 1 */
	  {
	  printf("shortest path algorithm ");
	  ret = dhlon();
	  printf("working node");
	  ret = dhloff();
	  printf("(predecessor)(len): \n");
	  for (j = 0 ; j < numnodes ; j++) printf(" %3d   ",j);
	  printf("\n");
	  for (j = 0 ; j < numnodes ; j++) printf(" %5s ",nodes[j].name);
	  printf("\n");
	  for (i = 0 ; i < numnodes ; i++)
		{
		if (i == k) ret = dhlon();
		if (nodestate[i].predecess == -1)
		  printf(" **");
		else
		  printf(" %2d",nodestate[i].predecess);
		if (nodestate[i].len == INFTY )
		  printf(" INF");
		else
		  printf(" %3d",nodestate[i].len);
		if (i == k) dhloff();
		}
	  printf("\n");
	  }

repeat:	for (i = 0 ; i < numnodes ; i++) /* examine all tentative
					nodes which are adjacent to k 
					and reduces len's if possible */
		{
		if ((mat[k][i] != 0) && (mat[k][i] != INFTY ) &&
			(nodestate[i].perm == 0))
		  if (nodestate[k].len + mat[k][i] < nodestate[i].len)
		    {
		    nodestate[i].predecess = k;
		    nodestate[i].len = nodestate[k].len + mat[k][i];
		    }
		}

	m = INFTY ;                     /* find the tentatively labelled
					node with the smallest len */
	for (i = 0 ; i < numnodes ; i++)
		{
		if ((nodestate[i].perm == 0) && (nodestate[i].len < m))
		  {
		  m = nodestate[i].len;
		  k = i;            /* exit 'for' with new working node */
		  }
		}
	nodestate[k].perm = 1;      /* new working node */
	if (df == 1)
	  {
	  for (i = 0 ; i < numnodes ; i++)
		{
		if (i == k) ret = dhlon();
		if (nodestate[i].predecess == -1)
		  printf(" **");
		else
		  printf(" %2d",nodestate[i].predecess);
		if (nodestate[i].len == INFTY )
		  printf(" INF");
		else
		  printf(" %3d",nodestate[i].len);
		if (i == k) dhloff();
		}
	  printf("\n");
	  }

	if (k != src) goto repeat;  /* the 'infamous goto' */

	k = src;
	i = 0;
	sum = 0;
	while (k != -1)
		{
		path[i] = k;
		i = i + 1;
		k = nodestate[k].predecess;
		if (k != -1) sum = sum + mat[path[i-1]][k];
		}
	*pnum = i;

	return(sum);
	}  /* end of short_path */

/* ---------------------------------------------------------------- */

#if defined(_NOPROTO)
int min(i,j)           
int	i,j;
#else
int min(int i, int j)
#endif
			/* usual minimum function */
	{
	if (i < j) return(i); else return(j);

	}  /* end of min */

/* ---------------------------------------------------------------- */

#if defined(_NOPROTO)
walk(node)     
int	node;
#else
int walk(int node)
#endif
		/* this function recursively traverses the layered network
		      described in flayer[] and inverts layer numbers,
		      thus pruning 'dead ends'. */
	{
	int	lnode,j;

	if (fdisp == 1) printf(" %d",node);

	flayer[node] = -flayer[node];
	lnode = flayer[node];
	for (j = 0 ; j < numnodes ; j++)
		if ((j != fsrc) && (-flayer[j] == lnode - 1) &&
		 ((flow[j][node] < fcap[j][node]) ||
		 (flow[node][j] > 0)))
		 walk(j);

	return(0);
	}  /* end of walk */

/* ----------------------------------------------------------------- */

#if defined(_NOPROTO)
int layer_net()          
#else
int layer_net(void)
#endif
		/* this function tries, using flayer[], flow[][]
		and fcap[][], to construct a new layered
		network where each link is 'unsaturated',
		if possible. If not possible, it returns 0 . */
	{
	int	i,j,k,empty,possible;

	k = 0;				/* begin with layer 0 */
	for (i = 0 ; i < numnodes ; i++) flayer[i] = UNSCANNED ;
	flayer[fsrc] = k;		/* source node is in layer 0 */

	do
	  {
	  k = k + 1;			/* locate all nodes in layer k */
	  empty = 1;
	  for (i = 0 ; i < numnodes ; i++)
	    {
	    if (-flayer[i] == k-1)
	      for (j = 0 ; j < numnodes ; j++)
		if ((flayer[j] == UNSCANNED ) &&
		    ((flow[i][j] < fcap[i][j]) || (flow[j][i] > 0)))
		    {
		    flayer[j] = -k;
		    empty = 0;
		    }
	    }  /* end of for */
	  } while ((flayer[fdest] == UNSCANNED) && (empty == 0));
	if (empty == 1) possible = 0; else possible = 1;

	if (fdisp == 1) printf("recursively walking: ");
	walk(fdest);	   		 /* prune off the dead ends */
	if (fdisp == 1) printf("\n");

	if ((fdisp == 1) && (possible == 1))
	  {
	  printf("new layered network is: \n");
	  for (j = 0 ; j < numnodes ; j++) printf("  %5s",nodes[j].name);
	  printf("\n");
	  for (i = 0 ; i < numnodes ; i++)
		{
		if (flayer[i] == UNSCANNED )
		  printf(" UNSCAN");
		else
		  printf("    %3d",flayer[i]);
		}
	  printf("\n");
	  }
	else if((fdisp == 1) && (possible == 0))
	  printf("new layered network not possible \n");
	  
	return(possible);
	}  /* end of layer_net */

/* ----------------------------------------------------------------- */

#if defined(_NOPROTO)
find_ref(node)            
int	node;
#else
int find_ref(int node)
#endif
		  /* this function recursively traverses network from
		     fdest, seeking the reference node (which
		     is the node with the smallest potential
		     flow increase) setting the variables
		     frefnode and fminpot in the process */
	{
	int	j,lnode,lj,incap,outcap;

	if (fdisp == 1) printf(" %d",node);

	lnode = flayer[node];
	incap = 0;
	outcap = 0;
	for (j = 0 ; j < numnodes ; j++)  /* examine each adjacent node */
	  {
	  lj = flayer[j];
	  if ((lj == lnode - 1) && (j != fsrc) &&
	      ((flow[j][node] < fcap[j][node]) || (flow[node][j] > 0)))
		find_ref(j);
	  if (lj == lnode - 1) 
	   incap = incap + (fcap[j][node] - flow[j][node]) + flow[node][j];
	  if (lj == lnode + 1)
	   outcap = outcap + (fcap[node][j] - flow[node][j]) + flow[j][node];
	  }  /* end of for */
	if ((node != fsrc) && (node != fdest) &&
	    (min(incap,outcap) < fminpot))
		{
		fminpot = min(incap,outcap);
		frefnode = node;
		}  /* note: min(incap,outcap) is potential flow increase
			through node */

	return(0);
	}  /* end of find_ref */

/* ---------------------------------------------------------------- */

#if defined(_NOPROTO)
int push_pull(node,flowleft,way)    
int	node,flowleft,way;
#else
int push_pull(int node, int flowleft, int way)
#endif

			/* this function tries to augment
			   flow[][] either by pushing additional
			   flow from node thru higher layers
			   toward the dest or by pulling
			   additional flow from src thru lower
			   layers toward node. Amount avail. is
			   flowleft units. If way = '1' it
			   pushes, if way = '0' it pulls */
	{
	int	j,k1,k2,lsought,r;

	j = -1;				/* initialize */
	while ((flowleft > 0) && (j < numnodes - 1))
	  {
	  j = j + 1;
	  if (way == 1)			/* push */
	    {
	    k1 = node;
	    k2 = j;
	    lsought = flayer[node] + 1;
	    }
	  else				/* pull */
	    {
	    k1 = j;
	    k2 = node;
	    lsought = flayer[node] -1;
	    }
	  r = min(flowleft,(fcap[k1][k2] - flow[k1][k2] + flow[k2][k1]));

/*	  printf("trying j = %d lsought = %d k1 = %d k2 = %d r = %d \n",
		j,lsought,k1,k2,r);   */

	  /* above is the amount of flow to move */
	  if ((r > 0) && (flayer[j] == lsought))
	    {
	    flowleft = flowleft - r;	/* push/pull some flow from
					   an adjacent layer */
	    flow[k1][k2] = flow[k1][k2] + r - min(r,flow[k2][k1]);
					/* augment positive flow */
	    flow[k2][k1] = flow[k2][k1] - min(r,flow[k2][k1]);
					/* push reverse flow back */
	    if ((j != fsrc) && (j != fdest))
		push_pull(j,r,way);
	    }
	  }  /* end of while */

	return(0);
	}  /* end of push_pull */

/* ----------------------------------------------------------------- */

#if defined(_NOPROTO)
int max_flow(d,s,mat,df)      
int	d,s,df;
int	mat[][ MAXNODES ];
#else
int max_flow(int d, int s, int mat[][ MAXNODES ], int df)
#endif
			/* this function leaves the maximum flow route
		     from node s to node d in matrix flow[][]
		     using the capacity matrix mat[][].
		     If df is set it displays the
		     progress of the algorithm. it also
		     returns the total maximum flow.  */
	{
	int	i,j,sum;

	sum = 0;
	fdest = d;                       /* initialize common areas */
	fsrc = s;
	fdisp = df;
	for (i = 0 ; i < numnodes ; i++)
	  for (j = 0 ; j < numnodes ; j++)
		{
		fcap[i][j] = mat[i][j];
		flow[i][j] = 0;               /* initially no flow */
		}
	flow[fsrc][fdest] = fcap[fsrc][fdest]; /* if direct link, use it */

	while (layer_net() == 1)  /* refine layered network as long
				     as possible */
	  {
	  fminpot = INFTY ;       /* start with no limits */

	  if (fdisp == 1) printf("recursive find_ref calls: ");
	  find_ref(fdest);      /* find reference node, put in fref_node */
	  if (fdisp == 1) printf("\n");
	  if (fdisp == 1)
	    printf("refnode = %d  with smallest potential = %d \n",
		frefnode,fminpot);

	  if (fdisp == 1) printf("now pushing toward destination. ");
	  push_pull(frefnode,fminpot,1);      /* '1' = push toward dest */

/*	  printf("flow matrix after push: \n");
 	  print_mat(flow);  */

	  if (fdisp == 1) printf("now pulling from source. \n");
	  push_pull(frefnode,fminpot,0);      /* '0' = pull from src */

/*	  printf("flow matrix after pull: \n");
	  print_mat(flow);  */
	  }

	/* at this point we have a valid flow[][] matrix and can find
	   the max flow by either adding flow from source or adding
	   flow into dest */

	for (i = 0 ; i < numnodes ; i++)
	  sum = sum + flow[fsrc][i];

	return(sum);
	}  /* end max_flow */

/* ------------------------------------------------------------------
VT100 screen handling routines: 	dinit()
					dmargin()
					dclscrn()
					dcl2eos()
					dcl2eol()
					dmove()
					dhlon()
					dhloff()
					dbold()
					dnobold()
					dgetesc()
					dcrlf()
					dbell()
					dpush()
					dpop()
 ----------------------------------------------------------------------- */

#if defined(_NOPROTO)
int dinit()  
#else
int dinit(void)
#endif		
		/* This function sets up an ANSI terminal as follows:
             1. set keypad application  ESC=               */
	{
	putchar(033);
	putchar('=');
	return(0);
	}  /* end of dinit */

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
dmargin(top,bottom)
int	top,bottom;
#else
int dmargin(int top, int bottom)
#endif
			/* This function sets margins for scrolling
	   region. The cursor is placed at HOME.  Note that this is a
	   non-ANSI, DEC PRIVATE VT100 function.
		   ESC[top;bottomr  */
	{
	int	rem1, quot1, rem2, quot2;

	rem1 = top % 10;
	quot1 = (top - rem1)/10;
	rem2 = bottom % 10;
	quot2 = (bottom - rem2)/10;

	if (top > 0 && top < 25 && bottom > 0 && bottom < 25)
		{
		rem1 = rem1 + 48;  /* convert to ascii */
		quot1 = quot1 + 48;
		rem2 = rem2 + 48;
		quot2 = quot2 + 48;
		putchar(033);
		putchar('[');
		putchar(quot1);
		putchar(rem1);
		putchar(';');
		putchar(quot2);
		putchar(rem2);
		putchar('r');
		}
	return(0);
	}  /* end of dmargin */

/* -------------------------------------------------------------------- */

#if defined(_NOPROTO)
int dclscrn()  
#else
int dclscrn(void)
#endif
		/* This function clears the entire 24 x 80 screen 
              and HOMEs the cursor    ESC[;H ESC[2J          */
	{
	putchar(033);
	putchar('[');
	putchar(';');
	putchar('H');
	putchar(033);
	putchar('[');
	putchar('2');
	putchar('J');
	return(0);
	}  /* end of dclscrn */

/* ---------------------------------------------------------------- */

#if defined(_NOPROTO)
int dcl2eos()  
#else
int dcl2eos(void)
#endif
			/* This function clears to the end of the
              screen    ESC[J    */
	{
	putchar(033);
	putchar('[');
	putchar('J');
	return(0);
	}  /* end of dcl2eos */

/* ---------------------------------------------------------------- */

#if defined(_NOPROTO)
int dcl2eol()  
#else
int dcl2eol(void)
#endif
			/* This function clears to the end of the
              line      ESC[K    */
	{
	putchar(033);
	putchar('[');
	putchar('K');
	return(0);
	}  /* end of dcl2eol */

/* ---------------------------------------------------------------- */

#if defined(_NOPROTO)
dmove(row,col)  
int	row, col;
#else
int dmove(int row, int col)
#endif
			/* This function moves the screen cursor to the
                   specified line (1-24) and col(1-80). because
                   wraparound is not always precisely specified
                   for ansi compatibility it is best to avoid
                   assumptions about col 80.     ESC[row;colH   */
	{
	int	rem1, quot1, rem2, quot2;

	rem1 = row % 10;
	quot1 = (row - rem1)/10;
	rem2 = col % 10;
	quot2 = (col - rem2)/10;

	if (row > 0 && row < 25 && col > 0 && col < 81)
		{
		rem1 = rem1 + 48;  /* convert to ascii */
		quot1 = quot1 + 48;
		rem2 = rem2 + 48;
		quot2 = quot2 + 48;
		putchar(033);
		putchar('[');
		putchar(quot1);
		putchar(rem1);
		putchar(';');
		putchar(quot2);
		putchar(rem2);
		putchar('H');
		}
	return(0);
	}  /* end of dmove */

/* ---------------------------------------------------------------- */

#if defined(_NOPROTO)
int dhlon()    
#else
int dhlon(void)
#endif
		/* This function turns on highlight attribute. Note that
	straight VT100 terminals do not allow more than one attribute
	active at one time (so you CANNOT simultaneously use both
	highlight and bold)     ESC[7m  */
	{
	putchar(033);
	putchar('[');
	putchar('7');
	putchar('m');
	return(0);
	}  /* end of dhlon */

/* --------------------------------------------------------------- */

#if defined(_NOPROTO)
int dhloff()   
#else
int dhloff(void)
#endif
		/* This function turns highlight off    ESC[m  */
	{
	putchar(033);
	putchar('[');
	putchar('m');
	return(0);
	}  /* end of dhloff */

/* ------------------------------------------------------------------ */

#if defined(_NOPROTO)
int dbold()	   
#else
int dbold(void)
#endif		/* This function turns bold attribute on     ESC[1m   */
	{
	putchar(033);
	putchar('[');
	putchar('1');
	putchar('m');
	return(0);
	}  /* end of dbold */

/* ----------------------------------------------------------------- */

#if defined(_NOPROTO)
int dnobold()  
#else
int dnobold(void)
#endif
		/* This function turns bold off    ESC[m    */
	{
	putchar(033);
	putchar('[');
	putchar('m');
	return(0);
	}  /* end of dnobold */

/* ------------------------------------------------------------------ */

#if defined(_NOPROTO)
char dgetesc()   
#else
char dgetesc(void)
#endif
			/* This function is called after an   ESC (033) has
	               been received. it gets two more characters in
	               transparent mode and returns the second character.
		       You MUST be in RAW mode, otherwise unwanted
		       characters will be echoed. Only 8 types of ESCape
		       sequences are expected:
	                 UP   (f1)      esc[A  or escOA
                         DOWN (f2)      esc[B  or escOB
	                 LEFT (f3)      esc[D  or escOD
                         RIGHT(f4)      esc[C  or escOC
                         HOME (f5)(pf1)      escOP
	                      (f6)(pf2)      escOQ
	                      (f7)(pf3)      escOR
	                 ESC  (f8)(pf4)      escOS 
	               if invalid sequence is obtained '\0' is returned */ 
	{
	char	khar;

	khar = getchar();
	if (khar != '['  &&  khar != 'O') return('\0');
	khar = getchar();
	if (khar == 'A'  ||  khar == 'B'  ||  khar == 'C'  ||  khar == 'D'  ||
	    khar == 'P'  ||  khar == 'Q'  ||  khar == 'R'  ||  khar == 'S')
		{
		return(khar);
		}
	else return('\0');
	}  /* end of dgetesc */

/* ------------------------------------------------------------------ */

#if defined(_NOPROTO)
int dcrlf()   
#else
int dcrlf(void)
#endif
		/* This function sends a    CR    and a   LF   */
	{
	putchar(015);  /* cr is ascii 13 */
	putchar(012);  /* lf is ascii 10 */
	return(0);
	}  /* end of dcrlf */

/* ------------------------------------------------------------------ */

#if defined(_NOPROTO)
int dbell()
#else
int dbell(void)
#endif   
		/* This function sends a    BEL    to the terminal */
	{
	putchar(007);  /* bel is ascii 7 */
	return(0);
	}  /* end of dbell */

/* ----------------------------------------------------------------- */

#if defined(_NOPROTO)
int dpush()   
#else
int dpush(void)
#endif
		/* This function saves ANSI cursor position, etc. */
	{
	putchar(033);
	putchar('7');
	return(0);
	}  /* end of dpush */

/* ---------------------------------------------------------------- */

#if defined(_NOPROTO)
int dpop()    
#else
int dpop(void)
#endif
		/* This function restores ANSI cursor position, etc. */
	{
	putchar(033);
	putchar('8');
	return(0);
	}  /* end of dpop */

