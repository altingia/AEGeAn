#include "AgnGtExtensions.h"
#include "AgnTranscriptClique.h"
#include "AgnUtils.h"

//----------------------------------------------------------------------------//
// Data structure definition
//----------------------------------------------------------------------------//
struct AgnTranscriptClique
{
  GtDlist *transcripts;
  GtDlistelem *current;
};

typedef struct
{
  GtHashmap *map;
  bool idfound;
} TranscriptIdCheckData;

typedef struct
{
  FILE *outstream;
  GtFeatureNode *first;
} PrintIdsData;


//----------------------------------------------------------------------------//
// Private method prototypes
//----------------------------------------------------------------------------//

/**
 * Traversal function for determining length of CDS(s) for the transcript(s) in
 * this clique.
 *
 * @param[in]  transcript    transcript in the clique
 * @param[out] cdslength     pointer to an unsigned long for storing the CDS
 *                           length
 */
void clique_cds_length(GtFeatureNode *transcript, void *cdslength);

/**
 * Traversal function for copying the contents of one clique to another.
 *
 * @param[in]  transcript    transcript in the clique
 * @param[out] clique        new clique to which the transcript will be added
 */
void clique_copy(GtFeatureNode *transcript, void *clique);

/**
 * Traversal function for checking whether the clique includes one or more
 * transcripts whose IDs are associated with the provided data.
 *
 * @param[in]  transcript    transcript in the clique
 * @param[out] data          TranscriptIdCheckData, including a hashmap of IDs
 *                           and a boolean indicating whether the ID is in the
 *                           hashmap
 */
void clique_id_check(GtFeatureNode *transcript, void *data);

/**
 * Traversal function for placing all transcript IDs associated with this clique
 * in the provided hashmap.
 *
 * @param[in]  transcript    transcript in the clique
 * @param[out] map           hashmap of transcript IDs
 */
void clique_ids_put(GtFeatureNode *transcript, void *map);

/**
 * Traversal function for determining the number of exons belonging to this
 * transcript clique.
 *
 * @param[in]  transcript    transcript in the clique
 * @param[out] numexons      pointer to an unsigned long for tracking exon #
 */
void clique_num_exons(GtFeatureNode *transcript, void *numexons);

/**
 * Traversal function for determining the number of UTR segments belonging to
 * this transcript clique.
 *
 * @param[in]  transcript    transcript in the clique
 * @param[out] numutrs       pointer to an unsigned long for tracking UTR #
 */
void clique_num_utrs(GtFeatureNode *transcript, void *numutrs);

/**
 * Traversal function for copying contents of this clique to an array.
 *
 * @param[in]  transcript    transcript in the clique
 * @param[out] array         array to which transcripts will be added
 */
void clique_to_array(GtFeatureNode *transcript, void *array);


//----------------------------------------------------------------------------//
// Method implementations
//----------------------------------------------------------------------------//
void clique_cds_length(GtFeatureNode *transcript, void *cdslength)
{
  unsigned long *length = cdslength;
  *length += agn_gt_feature_node_cds_length(transcript);
}

void clique_copy(GtFeatureNode *transcript, void *clique)
{
  AgnTranscriptClique *cq = clique;
  agn_transcript_clique_add(cq, transcript);
}

void clique_id_check(GtFeatureNode *transcript, void *data)
{
  TranscriptIdCheckData *dat = data;
  const char *tid = gt_feature_node_get_attribute(transcript, "ID");
  if(gt_hashmap_get(dat->map, tid) != NULL)
    dat->idfound = true;
}

void clique_ids_put(GtFeatureNode *transcript, void *map)
{
  GtHashmap *mp = map;
  const char *tid = gt_feature_node_get_attribute(transcript, "ID");
  gt_assert(gt_hashmap_get(mp, tid) == NULL);
  gt_hashmap_add(map, (char *)tid, (char *)tid);
}

void clique_num_exons(GtFeatureNode *transcript, void *numexons)
{
  unsigned long *num = numexons;
  GtFeatureNode *current;
  GtFeatureNodeIterator *iter = gt_feature_node_iterator_new(transcript);
  for(current  = gt_feature_node_iterator_next(iter);
      current != NULL;
      current  = gt_feature_node_iterator_next(iter))
  {
    if(agn_gt_feature_node_is_exon_feature(current))
      *num += 1;
  }
  gt_feature_node_iterator_delete(iter);
}

void clique_num_utrs(GtFeatureNode *transcript, void *numutrs)
{
  unsigned long *num = numutrs;
  GtFeatureNode *current;
  GtFeatureNodeIterator *iter = gt_feature_node_iterator_new(transcript);
  for(current  = gt_feature_node_iterator_next(iter);
      current != NULL;
      current  = gt_feature_node_iterator_next(iter))
  {
    if(agn_gt_feature_node_is_utr_feature(current))
      *num += 1;
  }
  gt_feature_node_iterator_delete(iter);
}

void clique_print_ids(GtFeatureNode *transcript, void *data)
{
  PrintIdsData *dat = data;
  const char *id = gt_feature_node_get_attribute(transcript, "ID");
  fprintf(dat->outstream, "%s", id);
  if(transcript != dat->first)
    fputc(',', dat->outstream);
}

void clique_to_array(GtFeatureNode *transcript, void *array)
{
  GtArray *ar = (GtArray *)array;
  gt_array_add(ar, transcript);
}

void agn_transcript_clique_add(AgnTranscriptClique *clique,
                               GtFeatureNode *transcript)
{
  gt_dlist_add(clique->transcripts, transcript);
  agn_transcript_clique_reset(clique);
}

unsigned long agn_transcript_clique_cds_length(AgnTranscriptClique *clique)
{
  unsigned long length = 0;
  agn_transcript_clique_traverse(clique, (AgnCliqueVisitFunc)clique_cds_length,
                                 &length);
  return length;
}

AgnTranscriptClique* agn_transcript_clique_copy(AgnTranscriptClique *clique)
{
  AgnTranscriptClique *new = agn_transcript_clique_new();
  agn_transcript_clique_traverse(clique, (AgnCliqueVisitFunc)clique_copy, new);
  return new;
}

void agn_transcript_clique_delete(AgnTranscriptClique *clique)
{
  gt_dlist_delete(clique->transcripts);
  gt_free(clique);
  clique = NULL;
}

bool agn_transcript_clique_has_id_in_hash(AgnTranscriptClique *clique,
                                          GtHashmap *map)
{
  TranscriptIdCheckData data = { map, false };
  agn_transcript_clique_traverse(clique, (AgnCliqueVisitFunc)clique_id_check,
                                 &data);
  return data.idfound;
}

AgnTranscriptClique* agn_transcript_clique_new()
{
  AgnTranscriptClique *clique = gt_malloc(sizeof(AgnTranscriptClique));
  clique->transcripts = gt_dlist_new((GtCompare)gt_genome_node_cmp);
  agn_transcript_clique_reset(clique);
  return clique;
}

GtFeatureNode* agn_transcript_clique_next(AgnTranscriptClique *clique)
{
  if(clique->current == NULL)
  {
    agn_transcript_clique_reset(clique);
    return NULL;
  }
  GtFeatureNode *transcript = gt_dlistelem_get_data(clique->current);
  clique->current = gt_dlistelem_next(clique->current);
  return transcript;
}

unsigned long agn_transcript_clique_num_exons(AgnTranscriptClique *clique)
{
  unsigned long exon_count = 0;
  agn_transcript_clique_traverse(clique, (AgnCliqueVisitFunc)clique_num_exons,
                                 &exon_count);
  return exon_count;
}

unsigned long agn_transcript_clique_num_utrs(AgnTranscriptClique *clique)
{
  unsigned long utr_count = 0;
  agn_transcript_clique_traverse(clique, clique_num_utrs, &utr_count);
  return utr_count;
}

void agn_transcript_clique_print_ids(AgnTranscriptClique *clique,
                                     FILE *outstream)
{
  if(gt_dlist_size(clique->transcripts) == 1)
  {
    GtDlistelem *elem = gt_dlist_first(clique->transcripts);
    GtFeatureNode *transcript = gt_dlistelem_get_data(elem);
    fprintf(outstream, "%s", gt_feature_node_get_attribute(transcript, "ID"));
    return;
  }
  fprintf(outstream, "[");
  GtDlistelem *first = gt_dlist_first(clique->transcripts);
  PrintIdsData data = { outstream, gt_dlistelem_get_data(first) };
  agn_transcript_clique_traverse(clique, (AgnCliqueVisitFunc)clique_print_ids,
                                 &data);
  fprintf(outstream, "]");

}

void agn_transcript_clique_put_ids_in_hash(AgnTranscriptClique *clique,
                                           GtHashmap *map)
{
  agn_transcript_clique_traverse(clique, clique_ids_put, map);
}

void agn_transcript_clique_reset(AgnTranscriptClique *clique)
{
  if(gt_dlist_size(clique->transcripts) == 0)
    clique->current = NULL;
  else
    clique->current = gt_dlist_first(clique->transcripts);
}

unsigned long agn_transcript_clique_size(AgnTranscriptClique *clique)
{
  return gt_dlist_size(clique->transcripts);
}

GtArray* agn_transcript_clique_to_array(AgnTranscriptClique *clique)
{
  GtArray *new = gt_array_new( sizeof(GtFeatureNode *) );
  agn_transcript_clique_traverse(clique, (AgnCliqueVisitFunc)clique_to_array,
                                 new);
  return new;
}

void agn_transcript_clique_traverse(AgnTranscriptClique *clique,
                                    AgnCliqueVisitFunc func, void *funcdata)
{
  gt_assert(func);
  GtDlistelem *elem;
  for(elem = gt_dlist_first(clique->transcripts);
      elem != NULL;
      elem = gt_dlistelem_next(elem))
  {
    GtFeatureNode *transcript = gt_dlistelem_get_data(elem);
    func(transcript, funcdata);
  }
}
