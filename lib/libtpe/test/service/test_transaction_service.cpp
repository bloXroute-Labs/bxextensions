#include <gtest/gtest.h>

#include <tpe/service/transaction_service.h>
#include <utils/protocols/blockchain_protocol.h>
#include <utils/common/string_helper.h>

typedef utils::common::BufferView TxContents_t;
typedef std::shared_ptr<TxContents_t> PTxContents_t;
typedef service::TransactionService TransactionService_t;
typedef service::TxProcessingResult TxProcessingResult_t;
typedef service::Sha256_t Sha256_t;

#define SHORT_ID 16


class TransactionServiceTest : public ::testing::Test {
public:

    TransactionServiceTest(): _tx_service(1, BLOCKCHAIN_PROTOCOL_ETHEREUM) {
    }

	static const Sha256_t sha256;

	TransactionService_t& tx_service() {
		return _tx_service;
	}

protected:
	void SetUp() override {
		_tx_service.get_tx_hash_to_short_ids()[sha256].insert(SHORT_ID);
	}

private:
	TransactionService_t _tx_service;
};

const Sha256_t TransactionServiceTest::sha256 =
		Sha256_t(
				"5a77d1e9612d350b3734f6282259b7ff0a3f87d62cfef5f35e91a5604c0490a3"
		);

TEST_F(TransactionServiceTest, test_has_short_id) {
	Sha256_t missing_sha("5a77d1e9612d350b3734f6282259b7ff0a3f87d62cfef5f35e91a5604c0490a8");
	EXPECT_TRUE(tx_service().has_short_id(sha256));
	EXPECT_FALSE(tx_service().has_short_id(missing_sha));
}

TEST_F(TransactionServiceTest, test_get_short_id) {
	EXPECT_EQ(tx_service().get_short_id(sha256), SHORT_ID);
}

TEST_F(TransactionServiceTest, test_clear) {
    tx_service().clear();
}

TEST_F(TransactionServiceTest, test_clear_sender_nonce) {
    std::string tx_str = "f866820321850e6f7cec008255f0942e059e37367ba9f66a0f75ad17f3a9eb7ed8d063808025a01e7c52081d11b9afefd88c03a030b39ea19214a37a1ac3f075c577b8d24ef0fea0097db5c8310b06517bef8fc9c6d31d5f358d20d240426fac1cba6818ad77b39b";
    std::vector<uint8_t> vec;
    utils::common::from_hex_string(tx_str, vec);
    TxContents_t tx = TxContents_t(&vec.at(0), vec.size());
    tx_service().set_sender_nonce_reuse_setting(2.0, 1.1);
    PTxContents_t p_tx = std::make_shared<TxContents_t>(tx);
    double current_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    TxProcessingResult_t result = tx_service().process_transaction_msg(
        Sha256_t("ba411636011e34ce464f39124b90394eef3e6be0ce9efbe6ec3a8bd98a9a9b6f"),
        p_tx,
        0,
        0.0,
        0.01,
        true,
        0,
        true
    );
    EXPECT_EQ(result.get_tx_validation_status(), TX_VALIDATION_STATUS_VALID_TX);

    result = tx_service().process_transaction_msg(
        Sha256_t("33411636011e34ce464f39124b90394eef3e6be0ce9efbe6ec3a8bd98a9a9b6f"),
        p_tx,
        0,
        0.0,
        0.01,
        true,
        0,
        true
    );
    EXPECT_TRUE(has_status_flag(result.get_tx_status(), TX_STATUS_IGNORE_SEEN));
    EXPECT_EQ(result.get_tx_validation_status(), TX_VALIDATION_STATUS_REUSE_SENDER_NONCE);

    result = tx_service().process_transaction_msg(
        Sha256_t("33411636011e34ce464f39124b90394eef3e6be0ce9efbe6ec3a8bd98a9a9b6f"),
        p_tx,
        1,
        0.0,
        0.01,
        true,
        0,
        false
    );

    EXPECT_EQ(tx_service().clear_sender_nonce(current_time), 1);
    EXPECT_EQ(tx_service().clear_sender_nonce(current_time), 0);
}


