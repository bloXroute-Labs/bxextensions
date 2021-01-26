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
    std::string tx_str = "f86682032a850a02ffee008255f0942e059e37367ba9f66a0f75ad17f3a9eb7ed8d063808026a0e3186d7f0954318d84d97d4065cbbd5546bc3aee21665869b8230960efca9bf5a034f6563ec353a111fb3ac14797d0b31a3b9f7ba0f9df139d91d8477a0d5e89cb";
    std::vector<uint8_t> vec;
    utils::common::from_hex_string(tx_str, vec);
    TxContents_t tx = TxContents_t(&vec.at(0), vec.size());
    tx_service().set_sender_nonce_reuse_setting(2.0, 1.1);
    PTxContents_t p_tx = std::make_shared<TxContents_t>(tx);
    double current_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    TxProcessingResult_t result = tx_service().process_transaction_msg(
        Sha256_t("a22d2caf295623c67deb867d2c92be995e257295c3039c6bae281041cc5314a1"),
        p_tx,
        0,
        0.0,
        current_time,
        true,
        0,
        true
    );
    EXPECT_EQ(result.get_tx_validation_status(), TX_VALIDATION_STATUS_VALID_TX);

    tx_str = "f86682032a850a3e9ab8008255f0942e059e37367ba9f66a0f75ad17f3a9eb7ed8d063808026a0149db315c4ef51477674fae84d92a5bcf9a7681442d3bf10b63692e968420feca0357b8a9f8b9640d2c652e28f32efd4ab18c00e37fbdb279086a72d6d8af46118";
    utils::common::from_hex_string(tx_str, vec);
    tx = TxContents_t(&vec.at(0), vec.size());
    p_tx = std::make_shared<TxContents_t>(tx);

    result = tx_service().process_transaction_msg(
        Sha256_t("b02dce4a44a4ba7f873d3b8205c5689f89849141ee263adb75b9917e04e2712f,"),
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


