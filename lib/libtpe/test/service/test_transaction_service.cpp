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
    std::string tx_str = "f86424851faa3b50008255f0940000000000000000000000000000000000000000808026a079ee3941cedf9972a54af4434c69fa40d728f878d9ff9ff22e1b2c41c680a945a06c45f014266e8e4e5fd892af3ef1711def8eb2c58e5a5bfe344cd8c76b996147";
    std::vector<uint8_t> vec;
    utils::common::from_hex_string(tx_str, vec);
    TxContents_t tx = TxContents_t(&vec.at(0), vec.size());
    tx_service().set_sender_nonce_reuse_setting(2.0, 1.1);
    PTxContents_t p_tx = std::make_shared<TxContents_t>(tx);
    double current_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    TxProcessingResult_t result = tx_service().process_transaction_msg(
        Sha256_t("db8f1328c253b627754efe074fefd45ec93910b06510d2fee492e245765e704f"),
        p_tx,
        0,
        0.0,
        current_time,
        true,
        0,
        false
    );
    EXPECT_EQ(result.get_tx_validation_status(), TX_VALIDATION_STATUS_VALID_TX);

    tx_str = "f86424851fe5d61a008255f0940000000000000000000000000000000000000000808026a03a460273b722ed3ea69cfc074f0304d5460e2e7014452d5ce0fabb4ff4eb612aa0194b21e020e12faa6af69ea6fb2d3b5617c3e4c32d00ec1060a9deb93656ca8c";
    vec.clear();
    tx.empty();
    utils::common::from_hex_string(tx_str, vec);
    tx = TxContents_t(&vec.at(0), vec.size());
    p_tx = std::make_shared<TxContents_t>(tx);
    current_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    result = tx_service().process_transaction_msg(
        Sha256_t("22f77d235b62a51c59f15ef1956deadb43783b25a237818a84ea8b5660fc31b6,"),
        p_tx,
        0,
        0.0,
        current_time,
        true,
        0,
        true
    );
    EXPECT_EQ(result.get_tx_validation_status(), TX_VALIDATION_STATUS_REUSE_SENDER_NONCE);

    EXPECT_EQ(tx_service().clear_sender_nonce(current_time + 3), 1);
    EXPECT_EQ(tx_service().clear_sender_nonce(current_time + 3), 0);
}


