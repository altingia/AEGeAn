#include "AgnTypecheck.h"

bool agn_typecheck_cds(GtFeatureNode *fn)
{
  return gt_feature_node_has_type(fn, "CDS") ||
         gt_feature_node_has_type(fn, "coding sequence") ||
         gt_feature_node_has_type(fn, "coding_sequence");
}

bool agn_typecheck_exon(GtFeatureNode *fn)
{
  return gt_feature_node_has_type(fn, "exon");
}

bool agn_typecheck_intron(GtFeatureNode *fn)
{
  return gt_feature_node_has_type(fn, "intron");
}

bool agn_typecheck_mrna(GtFeatureNode *fn)
{
  return gt_feature_node_has_type(fn, "mRNA") ||
         gt_feature_node_has_type(fn, "messenger RNA") ||
         gt_feature_node_has_type(fn, "messenger_RNA");
}

bool agn_typecheck_transcript(GtFeatureNode *fn)
{
  return agn_typecheck_mrna(fn) ||
         gt_feature_node_has_type(fn, "tRNA") ||
         gt_feature_node_has_type(fn, "transfer RNA") ||
         gt_feature_node_has_type(fn, "rRNA") ||
         gt_feature_node_has_type(fn, "ribosomal RNA");
}

bool agn_typecheck_utr(GtFeatureNode *fn)
{
  return gt_feature_node_has_type(fn, "UTR") ||
         gt_feature_node_has_type(fn, "untranslated region") ||
         gt_feature_node_has_type(fn, "untranslated_region") ||
         agn_typecheck_utr3p(fn) ||
         agn_typecheck_utr5p(fn);
}

bool agn_typecheck_utr3p(GtFeatureNode *fn)
{
  return gt_feature_node_has_type(fn, "3' UTR") ||
         gt_feature_node_has_type(fn, "3'UTR") ||
         gt_feature_node_has_type(fn, "three prime UTR") ||
         gt_feature_node_has_type(fn, "three_prime_UTR") ||
         gt_feature_node_has_type(fn, "three prime untranslated region") ||
         gt_feature_node_has_type(fn, "three_prime_untranslated_region");
}

bool agn_typecheck_utr5p(GtFeatureNode *fn)
{
  return gt_feature_node_has_type(fn, "5' UTR") ||
         gt_feature_node_has_type(fn, "5'UTR") ||
         gt_feature_node_has_type(fn, "five prime UTR") ||
         gt_feature_node_has_type(fn, "five_prime_UTR") ||
         gt_feature_node_has_type(fn, "five prime untranslated region") ||
         gt_feature_node_has_type(fn, "five_prime_untranslated_region");
}