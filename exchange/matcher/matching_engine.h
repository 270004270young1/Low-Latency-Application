#pragma  once
#include "common/thread_utils.h"
#include "common/lf_queue.h"
#include "common/macros.h"
#include "order_server/client_request.h"
#include "order_server/client_response.h"
#include "market_data/market_update.h"
#include "me_order_book.h"

namespace Exchange{

    class MatchingEngine final{

        public:
            MatchingEngine(ClientRequestLFQueue *client_requests,ClientResponseLFQueue *client_responses, MEMarketUpdateLFQueue *market_updates);

            ~MatchingEngine();


            auto processClientRequest(const MEClientRequest *client_request) noexcept{
                auto order_book = ticker_order_book_[client_request->ticker_id];

                switch(client_request->type_){
                    case ClientRequestType::NEW:{
                        order_book->add(client_request->client_id_,client_request->order_id,client_request->ticker_id,client_request->side_,client_request->price_,client_request->qty_);

                    }
                        break;
                    case ClientRequestType::CANCEL:{
                        order_book->cancel(client_request->client_id,client_request->order_id,client_request->ticker_id);
                    }
                        break;
                    default:{
                        FATAL("Received invalid client-request-type:"+clientRequestTypeToString(client_request->type_));
                    }
                        break;
                }
            }

            auto sendClientResponse(const MEClientResponse *client_response) noexcept{

                logger_.log("%:% %()  % Sending %\n",__FILE__,__LINE__,__FUNCTION__,Common::getCurrentTimeStr(&time_str_),client_response->toString());
                auto next_write = outgoing_ogw_responses_->getNextToWriteTo();
                *next_write = std::move(*client_response);
                outgoing_ogw_responses_->updateWriteIndex();
                

            }

            auto start() -> void;
            auto stop() -> void;
            auto run() noexcept{
                logger_.log("%:%: %() %\n",__FILE__,__LINE__,__FUNCTION__,Common::getCurrentTimeStr(&time_str_));

                while(run_){
                    const auto me_client_request = incoming_requests_->getNextToRead();

                    if(LIKELY(me_client_request)){

                        logger_.log("%:% %() % Proccessing %\n",__FILE__,__LINE__,__FUNCTION__,Common::getCurrentTimeStr(&time_str_),me_client_request->toString());
                        proccessClientRequest(me_client_request);
                        incoming_requests_->updateReadIndex();

                    }
                }
            }


            MatchingEngine() = delete;
            MatchingEngine(const MatchingEngine &) = delete;
            MatchingEngine(const MatchingEngine &&) = delete;
            MatchingEngine &operator=(const MatchingEngine &) = delete;
            MatchingEngine &operator=(const MatchingEngine &&) = delete;
        
        private:
            OrderBookHashMap ticker_order_book_;
            ClientRequestLFQueue *incoming_requests_ = nullptr;
            ClientResponseLFQueue *outgoing_ogw_responses_ = nullptr;
            MEMarketUpdateLFQueue *outgoing_md_updates = nullptr;
            volatile bool run_ = false;
            std::string time_str_;
            Logger logger_;
    };

    

}