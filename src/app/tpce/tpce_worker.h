#ifndef NOCC_OLTP_TPCE_WORKER_H
#define NOCC_OLTP_TPCE_WORKER_H

#include "core/rworker.h"
#include "core/utils/spinbarrier.h"
#include "framework/bench_worker.h"

#include "rtx/occ.h"
#include "rtx/occ_rdma.h"

#include "tpce_schema.h"
#include "tpce_mixin.h"

/* TPCE-EGen generator */
#include "egen/CETxnInputGenerator.h"
#include "egen/EGenLoader_stdafx.h"
#include "egen/EGenGenerateAndLoad.h"
#include "egen/MiscConsts.h"

//#define SANITY_CHECKS

//#define OCC_TX
//#define RAD_TX
//#define SI_TX

#define TX_CP  0
#define TX_SED 1
#define TX_TL0 2
#define TX_TL1 3
#define TX_BV  4
#define TX_MW  5

#define RPC_ADD (MAX_RPC_SUPPORT - 1)

extern size_t current_partition;

namespace nocc {
namespace oltp {

namespace tpce {

#define META_LENGTH 16

/* customer position rpc related stuff */
struct CPInputHeader {
  uint16_t secs;
  uint64_t time;
};

struct CPInputItem { uint8_t pid; uint8_t idx0; uint8_t idx1;char name[24]; };
struct CPOutput    { uint8_t idx0; uint8_t idx1; double val;};

/* security detail rpc related stuff */
struct SEDInput  {
  char name[24];
#ifdef SI_TX
  uint64_t ts_vec[TS_VEC_NUM];
#else
  uint64_t time;
#endif
};
struct SEDOutput { double lt_price;double lt_open_price;int64_t lt_vol;};
/* trade lookup rpc stuff */
struct TradeLookupInput {
  char name[24];
  uint64_t start_dts;
  uint64_t end_dts;
#ifdef SI_TX
  uint64_t ts_vec[TS_VEC_NUM];
#else
  uint64_t time;
#endif
};

struct TradeLookupOutput1 {
  uint64_t trade_ids[TPCE::TradeLookupMaxRows];
  uint16_t  count;
#ifdef SI_TX
  uint64_t ts_vec[TS_VEC_NUM];
#else
  uint64_t time;
#endif
};

/* broke volumn input */
struct BrokerVolumnInput {
#ifdef SI_TX
  uint64_t ts_vec[TS_VEC_NUM];
#else
  uint64_t time;
#endif
  TPCE::TBrokerVolumeTxnInput1 input;
};

/* market feed input */
//    typedef CPInputHeader MarketWatchInput;
struct  MarketWatchInput {
#ifdef SI_TX
  uint64_t ts_vec[TS_VEC_NUM];
#else
  uint64_t time;
#endif
  int16_t secs;
  uint64_t start_day;
};
struct  MWInputItem { uint8_t pid; char name[16];};
struct  SK {
  char data[16];
  SK(const char *s) {
    memset(data,0,16);
    assert(strlen(s) < 16);
    NDB_MEMCPY(data,s,strlen(s));
  }
  SK(std::string &s) {
    assert(s.size() < 16);
    memset(data,0,16);
    NDB_MEMCPY(data,s.data(),s.size());
  }
};

void TpceTest(int argc,char **argv);

class TpceLoader : public BenchLoader, public TpceMixin {
 public:
  TpceLoader(unsigned long seed, MemDB *store);
 protected:
  virtual void load();
  // Used to test RDMA performance, which load a subset of tables for simplicity
  void simple_load();
};

class TpceWorker : public TpceMixin, public BenchWorker {
 public:
  TpceWorker(unsigned int worker_id,unsigned long seed,MemDB *store,
             spin_barrier *a,spin_barrier *b,BenchRunner *context);
  /* Tx implementations */
  txn_result_t txn_cp_new(yield_func_t &yield);

  /* helper rpcs */
  //void add_market_feed(int id,int cid,char *input,void *arg);

 private:
  TPCE::CCETxnInputGenerator *input_generator_;
  uint64_t get_last_trade_id();
  uint64_t trade_id_;

#if 0
  uint64_t mac_heatmap[16];
#endif

 public:
  virtual workload_desc_vec_t get_workload() const ;
  static  workload_desc_vec_t _get_workload();
  virtual void check_consistency();
  virtual void register_callbacks();
  virtual void thread_local_init();
  virtual void workload_report();

  /* Wrappers*/
  static txn_result_t TxnTradeOrder(BenchWorker *w,yield_func_t &yield) {
    return txn_result_t(false,1);
  }

  static txn_result_t TxnTradeResult(BenchWorker *w,yield_func_t &yield) {
    return txn_result_t(false,1);
  }

  static txn_result_t TxnCustomerPos(BenchWorker *w,yield_func_t &yield) {
    return txn_result_t(false,1);
  }

  static txn_result_t TxnBrokerVolume(BenchWorker *w,yield_func_t &yield) {
    return txn_result_t(false,1);
  }

  static txn_result_t TxnMarketWatch(BenchWorker *w,yield_func_t &yield) {
    return txn_result_t(false,1);
  }

  static txn_result_t TxnSecurityDetail(BenchWorker *w,yield_func_t &yield) {
    return txn_result_t(false,1);
  }

  static txn_result_t TxnTradeLookup(BenchWorker *w,yield_func_t &yield) {
    return txn_result_t(false,1);
  }

  static txn_result_t TxnTradeStatus(BenchWorker *w,yield_func_t &yield) {
    return txn_result_t(false,1);
  }

  static txn_result_t TxnMarketFeed(BenchWorker *w,yield_func_t &yield) {
    return txn_result_t(false,1);
  }

  static txn_result_t TxnTradeUpdate(BenchWorker *w,yield_func_t &yield) {
    return txn_result_t(false,1);
  }
};

} // namespace tpce
}
}
#endif
