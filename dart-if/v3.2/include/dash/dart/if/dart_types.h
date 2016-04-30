
#ifndef DART_TYPES_H_INCLUDED
#define DART_TYPES_H_INCLUDED

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DART_INTERFACE_ON

typedef enum
  {
    DART_OK           =   0,
    DART_PENDING      =   1,
    DART_ERR_INVAL    =   2,
    DART_ERR_NOTFOUND =   3,
    DART_ERR_NOTINIT  =   4,
    DART_ERR_OTHER    = 999
    /* add error codes as needed */
  } dart_ret_t;

typedef enum
  {
    DART_OP_UNDEFINED = 0,
    DART_OP_MIN,
    DART_OP_MAX,
    DART_OP_SUM,
    DART_OP_PROD,
    DART_OP_BAND,
    DART_OP_LAND,
    DART_OP_BOR,
    DART_OP_LOR,
    DART_OP_BXOR,
    DART_OP_LXOR
  } dart_operation_t;

typedef enum
  {
    DART_TYPE_UNDEFINED = 0,
    DART_TYPE_BYTE,
    DART_TYPE_SHORT,
    DART_TYPE_INT,
    DART_TYPE_UINT,
    DART_TYPE_LONG,
    DART_TYPE_ULONG,
    DART_TYPE_LONGLONG,
    DART_TYPE_FLOAT,
    DART_TYPE_DOUBLE
  } dart_datatype_t;

typedef int32_t dart_unit_t;
typedef int32_t dart_team_t;

#define DART_UNDEFINED_UNIT_ID ((dart_unit_t)(-1))
#define DART_UNDEFINED_TEAM_ID ((dart_team_t)(-1))

typedef enum
  {
    DART_LOCALITY_SCOPE_UNDEFINED  =   -1,
    /** Global locality scope, components may be heterogenous. */
    DART_LOCALITY_SCOPE_GLOBAL     =    0,
    /** Node-level locality scope, components may be heterogenous. */
    DART_LOCALITY_SCOPE_NODE       =  100,
    /** Locality in a group of hetereogenous components in different NUMA
     *  domains. */
    DART_LOCALITY_SCOPE_MODULE     =  200,
    /** Locality of homogenous components in different NUMA domains. */
    DART_LOCALITY_SCOPE_NUMA       =  300,
    /** Locality of homogenous components in the same NUMA domain at
     *  process-level, i.e. of a unit-addressable, homogenous entity.
     *  A single unit corresponds to a DART (e.g. MPI) process and can
     *  occupy multiple homogenous cores, e.g. for multithreading. */
    DART_LOCALITY_SCOPE_UNIT       =  400,
    /** Locality at physical processing level. Cannot be referenced by DART
     *  directly. */
    DART_LOCALITY_SCOPE_CORE       =  500
  } dart_locality_scope_t;

#define DART_LOCALITY_DOMAIN_TAG_MAX_SIZE ((int)(16))
#define DART_LOCALITY_HOST_MAX_SIZE       ((int)(30))
#define DART_LOCALITY_UNIT_MAX_CPUS       ((int)(8))

/*
 * A domain is a group of processing entities such as cores in a specific
 * NUMA domain or a Intel MIC entity.
 * Domains are organized in a hierarchy.
 * In this, a domain may consist of heterogenous child domains.
 * Processing entities in domains on the lowest locality level are
 * homogenous.
 *
 * Domains represent the actual hardware topology but also can represent
 * grouping from user-defined team specifications.
 *
 * Use cases:
 *
 * - To determine whether units in a domain have access to common shared
 *   memory, test if domain descriptor field
 *
 *     - \c num_nodes is set to 1, or
 *     - \c scope is set to \c DART_LOCALITY_SCOPE_NODE or greater.
 *
 *
 * - The maximum number of threads for a single unit, e.g. for MKL routines,
 *   can be calculated as:
 *
 *     <tt>
 *       (dloc.num_cores x dloc.num_threads)
 *     </tt>
 *
 *   from a domain descriptor \c dloc with scope \c DART_LOCALITY_SCOPE_UNIT.
 *
 *
 * - A simple metric of processing power of components in a homogenous domain
 *   (minimum number of instructions per second) can be calculated as:
 *
 *     <tt>
 *       dmhz(dloc) = (dloc.num_cores x dloc.min_threads x dloc.min_cpu_mhz)
 *     </tt>
 *
 *   This metric then can be used to balance workload between homogenous
 *   domains with different processing components.
 *   A simple balance factor \c wb can be calculated as:
 *
 *     <tt>
 *       wb = dmhz(dloc_a) / dmhz(dloc_b)
 *     </tt>
 *
 *   from domain descriptors \c dloc_a and \c dloc_b.
 *
 *
 * Illustrating example:
 *
 * <tt>
 *   domain (top level, heterogenous)
 *   domain_tag:  "."
 *   host:        "number-crunch-9000"
 *   scope:       DART_LOCALITY_SCOPE_GLOBAL
 *   level:         0
 *   num_nodes:     4
 *   num_cores:   544 (4 nodes x 136 cores per node)
 *   min_threads:   2
 *   max_threads:   4
 *   num_domains:   4 (4 nodes)
 *   domains:
 *   :
 *   |-- domain (compute node, heterogenous)
 *   :   domain_tag:  ".0"
 *   :   scope:       DART_LOCALITY_SCOPE_NODE
 *   :   level:         1
 *   :   num_nodes:     1
 *   :   num_cores:   136 (16 host cores + 2x60 MIC cores)
 *   :   min_threads:   2
 *   :   max_threads:   4
 *   :   num_domains:   3 (1 host + 2 MICs)
 *   :   domains:
 *   :   :
 *   :   |-- domain (host, homogenous)
 *   :   :   domain_tag:  ".0.0"
 *   :   :   scope:       DART_LOCALITY_SCOPE_PROC_GROUP
 *   :   :   level:         2
 *   :   :   num_nodes:     1
 *   :   :   num_numa:      2
 *   :   :   num_cores:    16
 *   :   :   min_threads:   2
 *   :   :   max_threads:   2
 *   :   :   num_domains:   2
 *   :   :   :
 *   :   :   |-- domain (NUMA domain at host)
 *   :   :   :   domain_tag:  ".0.0.1"
 *   :   :   :   scope:       DART_LOCALITY_SCOPE_UNIT
 *   :   :   :   level:        3
 *   :   :   :   num_nodes:    1
 *   :   :   :   num_numa:     1
 *   :   :   :   num_cores:    8
 *   :   :   :   num_domains:  8
 *   :   :   :   :
 *   :   :   :   '
 *   :   :   :   ...
 *   :   :   :
 *   :   :   '-- domain (NUMA domain at host)
 *   :   :       domain_tag:  ".0.0.1"
 *   :   :       scope:       DART_LOCALITY_SCOPE_UNIT
 *   :   :       level:        3
 *   :   :       num_nodes:    1
 *   :   :       num_numa:     1
 *   :   :       num_cores:    8
 *   :   :       num_domains:  8
 *   :   :       :
 *   :   :       '
 *   :   :       ...
 *   :   :
 *   :   |-- domain (MIC, homogenous)
 *   :   :   domain_tag:  ".0.1"
 *   :   :   scope:       DART_LOCALITY_SCOPE_PROC_GROUP
 *   :   :   level:         2
 *   :   :   num_nodes:     1
 *   :   :   num_cores:    60
 *   :   :   min_threads:   4
 *   :   :   max_threads:   4
 *   :   :   num_domains:   0
 *   :   :
 *   :   '-- domain (MIC, homogenous)
 *   :       domain_tag:  ".0.2"
 *   :       scope:       DART_LOCALITY_SCOPE_PROC_GROUP
 *   :       level:         2
 *   :       num_nodes:     1
 *   :       num_cores:    60
 *   :       min_threads:   4
 *   :       max_threads:   4
 *   :       domains:
 *   :       num_domains:   2
 *   :       :
 *   :       |-- domain (unit of MIC cores, homogenous)
 *   :       :   domain_tag:  ".0.2.0"
 *   :       :   scope:       DART_LOCALITY_SCOPE_UNIT
 *   :       :   level:        3
 *   :       :   num_nodes:    1
 *   :       :   num_cores:   30
 *   :       :   num_domains:  0
 *   :       :
 *   :       '-- domain (unit of MIC cores, homogenous)
 *   :           domain_tag:  ".0.2.1"
 *   :           scope:       DART_LOCALITY_SCOPE_UNIT
 *   :           level:        3
 *   :           num_nodes:    1
 *   :           num_cores:   30
 *   :           num_domains:  0
 *   :
 *   |-- domain (compute node, heterogenous)
 *   :   domain_tag:  ".1"
 *   :   scope:       DART_LOCALITY_SCOPE_NODE
 *   :   level:         1
 *   :   num_cores:   136
 *   :   num_domains:   3
 *   :   domains:
 *   :   :
 *   :   '
 *   :   ...
 *   '
 *   ...
 * </tt>
 *
 */
struct dart_domain_locality_s
  {
    /**
     * Hierarchical domain identifier, represented as dot-separated string.
     * Example:
     *   0.5.5
     */
    char domain_tag[DART_LOCALITY_DOMAIN_TAG_MAX_SIZE];

    /** Locality scope of the domain. */
    dart_locality_scope_t scope;
    /** Level in the domain locality hierarchy. */
    int  level;

    /** Hostname of the domain's node or 0 if unspecified. */
    char host[DART_LOCALITY_HOST_MAX_SIZE];

    /** Pointer to descriptor of parent domain or 0 if no parent domain
     *  is specified. */
    struct dart_domain_locality_s * parent;

    /** Number of subordinate domains. */
    int  num_domains;
    /** Array of subordinate domains of size \c num_domains or 0 if no
     *  subdomains are specified. */
    struct dart_domain_locality_s * domains;

    /** Total number of NUMA domains in the associated domain. */
    int   num_numa;
    /** IDs of the unit's NUMA domains, relative to parent node domain. */
    int * numa_ids;

    /** Total number of CPUs in the associated domain. */
    int   num_cores;
    /** IDs of CPUs in the unit, relative to parent node domain. */
    int * cpu_ids;

    /** Number of compute nodes in the associated domain. */
    int   num_nodes;
    /** Number of processing groups (e.g. host + accelerators) per node. */
    int   num_modules;
    /** Total number of sockets per node. */
    int   num_sockets;

    /** Minimum clock frequency of CPUs in the domain. */
    int   min_cpu_mhz;
    /** Maximum clock frequency of CPUs in the domain. */
    int   max_cpu_mhz;

    /** Minimum number of threads per core in the domain. */
    int   min_threads;
    /** Maximum number of threads per core in the domain. */
    int   max_threads;

    /** Cache sizes in the unit by cache level (L1, L2, L3). */
    int   cache_sizes[3];
    /** Cache line sizes in the unit by cache level (L1, L2, L3). */
    int   cache_line_sizes[3];
    /** Flags indicating shared caches by cache level (L1, L2, L3). */
    int   cache_shared[3];

  };
struct dart_domain_locality_s;
typedef struct dart_domain_locality_s
  dart_domain_locality_t;

/**
 * Locality and topology information of a single unit.
 * Processing entities grouped in a single unit are homogenous.
 * Each unit is a member of one specific locality domain.
 */
typedef struct
  {
    /** Global unit ID */
    dart_unit_t unit;

    /** Identifier of the unit's parent homogenous locality domain. */
    char  domain_tag[DART_LOCALITY_DOMAIN_TAG_MAX_SIZE];

    /** Hostname of the unit's local node or 0 if unspecified. */
    char  host[DART_LOCALITY_HOST_MAX_SIZE];

    /** ID of the unit's NUMA domain, relative to parent processing module. */
    int   numa_id;

    /** The unit's affine core, unique identifier within a processing
     *  module. */
    int   core_id;

    /** Number of cores in the unit. */
    int   num_cores;

    /** Number of threads supported by a single core in the unit. */
    int   num_threads;
    /** IDs of \c (num_threads x num_cores) CPUs in the unit, relative to
     *  domain at node level. */
    int   cpu_ids[DART_LOCALITY_UNIT_MAX_CPUS];

    /** Minimum clock frequency of CPUs in the unit. */
    int   min_cpu_mhz;
    /** Maximum clock frequency of CPUs in the unit. */
    int   max_cpu_mhz;

  } dart_unit_locality_t;

#define DART_INTERFACE_OFF

#ifdef __cplusplus
}
#endif

#endif /* DART_TYPES_H_INCLUDED */
