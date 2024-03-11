#pragma
#include "common/types.h"
#include "common/mem_pool.h"
#include "common/logging.h"
#include "order_server/client_response.h"
#include "market_data/market_update.h"
#include "me_order.h"
using namespace Common;

namespace Exchange{

    class MatchingEngine;

    class MEOrderBook final{

        private:
            TickerId ticker_id_ = TickerId_INVALID;
            MatchingEngine *matching_engine_ = nullptr;
            ClientOrderHashMap cid_oid_to_order_;
            MemPool<MEOrdersAtPrice> orders_at_price_pool_;
            MEOrdersAtPrice *bids_by_price_ = nullptr; 
            MEOrdersAtPrice *asks_by_price_ = nullptr;
            OrdersAtPriceHashMap price_orders_at_price_;
            MemPool<MEOrder> order_pool_;
            MEClientResponse client_response_;
            MEMarketUpdate market_update_;
            OrderId next_market_order_id_ = 1;
            std::string time_str_;
            Logger *logger_ = nullptr;

            auto removeOrder(MEOrder *order) noexcept;

            auto removeOrdersAtPrice(Side side, Price price) noexcept;

        public:
            explicit MEOrderBook(TickerId ticker_id, Logger *logger, MatchingEngine *matching_engine);

            ~MEOrderBook();

            MEOrderBook() = delete;
            MEOrderBook(const MEOrderBook &) = delete;
            MEOrderBook(const MEOrderBook &&) = delete;
            MEOrderBook &operator=(const MEOrderBook &) = delete;
            MEOrderBook &operator=(const MEOrderBook &&) = delete;

            auto generateNewMarketOrderId() noexcept -> OrderId{
                return next_market_order_id_++;
            }

            auto priceToIndex(Price price) const noexcept{
                return (price % ME_MAX_PRICE_LEVELS);
            }

            auto getOrdersAtPrice(Price price) const noexcept -> MEOrdersAtPrice *{
                return price_orders_at_price_[priceToIndex(price)];
            }

            auto add(ClientId client_id, OrderId client_order_id, TickerId ticker_id, Side side, Price price, Qty qty) noexcept -> void;

            auto getNextPriority(Price price) noexcept;

            auto addOrder(MEOrder *order) noexcept;

            auto addOrdersAtPrice(MEOrdersAtPrice *new_orders_at_price) noexcept;

            auto cancel(ClientId client_id, OrderId order_id, TickerId ticker_id) noexcept -> void;

            auto checkForMatch(ClientId client_id, OrderId client_order_id, TickerId ticker_id, Side side, Price price, Qty qty, Qty new_market_order_id) noexcept;

            auto match(TickerId ticker_id, ClientId client_id, Side side, OrderId client_order_id, OrderId new_market_order_id, MEOrder *itr, Qty *leaves_qty) noexcept;

    };

    typedef std::array<MEOrderBook *, ME_MAX_TICKERS> OrderBookHashMap;
}