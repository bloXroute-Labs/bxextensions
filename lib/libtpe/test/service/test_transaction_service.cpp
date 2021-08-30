#include <gtest/gtest.h>
#include <utils/protocols/ethereum/eth_block_message.h>
#include <utils/protocols/ethereum/bx_eth_block_message.h>
#include <tpe/task/main_task_base.h>
#include <tpe/task/eth_block_compression_task.h>

#include "tpe/service/transaction_service.h"
#include "utils/protocols/blockchain_protocol.h"
#include "utils/common/string_helper.h"
#include "utils/common/buffer_helper.h"

typedef utils::common::BufferView BufferView_t;
typedef utils::common::ByteArray ByteArray_t;
typedef std::shared_ptr<ByteArray_t> PByteArray_t;
typedef std::shared_ptr<BufferView_t> PBufferView_t;
typedef service::TransactionService TransactionService_t;
typedef service::TxProcessingResult TxProcessingResult_t;
typedef service::Sha256_t Sha256_t;
typedef std::shared_ptr<service::BufferCopy_t> PBufferCopy_t;
typedef std::shared_ptr<TransactionService_t> PTransactionService_t;
typedef utils::common::BufferCopy BufferCopy_t;

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
    BufferView_t tx = BufferView_t(&vec.at(0), vec.size());
    tx_service().set_sender_nonce_reuse_setting(2.0, 1.1);
    PBufferView_t p_tx = std::make_shared<BufferView_t>(tx);
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
    tx = BufferView_t(&vec.at(0), vec.size());
    p_tx = std::make_shared<BufferView_t>(tx);
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


TEST_F(TransactionServiceTest, transactions_message_from_node) {
    std::string message_from_node_str = "f90101f85f800a82c35094095e7baea6a6c7c4c2dfeb977efac326af552d870a801ba09bea4c4daac7c7c52e093e6a4c35dbbcf8856f1af7b059ba20253e70848d094fa08a8fae537ce25ed8cb5af9adac3f141af69bd515bd2ba031522df09b97dd72b1b89e01f89b01800a8301e24194095e7baea6a6c7c4c2dfeb977efac326af552d878080f838f7940000000000000000000000000000000000000001e1a0000000000000000000000000000000000000000000000000000000000000000001a03dbacc8d0259f2508625e97fdfc57cd85fdd16e5821bc2c10bdd1a52649e8335a0476e10695b183a87b0aa292a7f4b78ef0c3fbe62aa2c42c84e1d9c3da159ef14";
    std::vector<uint8_t> vec;
    utils::common::from_hex_string(message_from_node_str, vec);
    BufferView_t message_from_node = BufferView_t(&vec.at(0), vec.size());
    PBufferView_t message_from_node_ptr = std::make_shared<BufferView_t>(message_from_node);

    PByteArray_t result = tx_service().process_gateway_transaction_from_node(
        message_from_node_ptr,
        0,
    true
    );
    BufferView_t result_bf(result->array(), result->size());
    size_t offset = 0;
    uint8_t seen_transaction;
    uint32_t tx_offset, tx_length, tx_status;
    Sha256_t tx_hash;
    uint32_t txn = 0;

    offset = utils::common::get_little_endian_value<uint32_t>(result_bf, txn, offset);
    ASSERT_EQ(2, txn);

    offset = utils::common::get_little_endian_value<uint8_t>(result_bf, seen_transaction, offset);
    tx_hash = Sha256_t(result_bf.vector(offset, 32), 0);
    offset += 32;
    offset = utils::common::get_little_endian_value<uint32_t>(result_bf, tx_offset, offset);
    offset = utils::common::get_little_endian_value<uint32_t>(result_bf, tx_length, offset);
    offset = utils::common::get_little_endian_value<uint32_t>(result_bf, tx_status, offset);
    ASSERT_EQ(0, seen_transaction);
    ASSERT_EQ("77b19baa4de67e45a7b26e4a220bccdbb6731885aa9927064e239ca232023215", tx_hash.hex_string());
    ASSERT_EQ(97, tx_length);
    ASSERT_EQ(TX_VALIDATION_STATUS_VALID_TX, tx_status);

    offset = utils::common::get_little_endian_value<uint8_t>(result_bf, seen_transaction, offset);
    tx_hash = Sha256_t(result_bf.vector(offset, 32), 0);
    offset += 32;
    offset = utils::common::get_little_endian_value<uint32_t>(result_bf, tx_offset, offset);
    offset = utils::common::get_little_endian_value<uint32_t>(result_bf, tx_length, offset);
    offset = utils::common::get_little_endian_value<uint32_t>(result_bf, tx_status, offset);
    ASSERT_EQ(0, seen_transaction);
    ASSERT_EQ("554af720acf477830f996f1bc5d11e54c38aa40042aeac6f66cb66f9084a959d", tx_hash.hex_string());
    ASSERT_EQ(160, tx_length);
    ASSERT_EQ(TX_VALIDATION_STATUS_VALID_TX, tx_status);

    ASSERT_EQ(result->size(), offset);
}

//TEST_F(TransactionServiceTest, eth_block_to_json) {
//    std::FILE* f = std::fopen("../../../../lib/libtpe/test/data/eth_sample_block", "r");
//
//    if (f == nullptr) {
//        f = std::fopen("../../../lib/libtpe/test/data/eth_sample_block", "r");
//    }
//
//    if (f == nullptr) {
//        f = std::fopen("lib/libtpe/test/data/eth_sample_block", "r");
//    }
//    ASSERT_TRUE(f != nullptr);
//    std::vector<char> buf(1024 * 1024); // char is trivally copyable
//    size_t chars = std::fread(&buf[0], sizeof buf[0], buf.size(), f);
//
//    std::string block_str (buf.begin(), buf.begin() + chars);
//    std::vector<uint8_t> vec;
//    utils::common::from_hex_string(block_str, vec);
//
//    BufferView_t block_buffer = BufferView_t(&vec.at(0), vec.size());
//    std::shared_ptr<ByteArray_t> block_json(tx_service().eth_block_to_json(block_buffer));
//}

TEST_F(TransactionServiceTest, eth_compress_block_type_2) {
    std::FILE* f = std::fopen("../../../../lib/libtpe/test/data/eth_sample_block", "r");

    if (f == nullptr) {
        f = std::fopen("../../../lib/libtpe/test/data/eth_sample_block", "r");
    }

    if (f == nullptr) {
        f = std::fopen("lib/libtpe/test/data/eth_sample_block", "r");
    }
    ASSERT_TRUE(f != nullptr);
    std::vector<char> buf(1024 * 1024); // char is trivially copyable
    size_t chars = std::fread(&buf[0], sizeof buf[0], buf.size(), f);
    std::string block_str (buf.begin(), buf.begin() + chars);

    std::vector<uint8_t> vec;
    utils::common::from_hex_string(block_str, vec);

    BufferView_t block_buffer = BufferView_t(&vec.at(0), vec.size());

    std::vector<uint8_t> block_msg_vec;
    utils::common::from_hex_string(block_str, block_msg_vec);
    BufferCopy_t block_message(BufferView_t(&block_msg_vec.at(0), block_msg_vec.size()));

    std::cout << "chars: " << chars << ", block_str.size(): " << block_str.size() << ", block_message.size(): " << block_message.size() << std::endl;
    std::shared_ptr<ByteArray_t> block_json(tx_service().eth_block_to_json(block_buffer));

    std::array<std::string, 291> tx_hashes {
        "85fc00ccbb990fd6e35bbbd7b21d75a84732eeb5f202f1cd14bc444375007639", "c3aa33c5f461b5d5abcf100e4568d9b4a04b8f6f7efad9e6db6ee999a04b538d",
        "0b661ea1250877027204e90a6181c0c5ae95d6ef91d367b69c99e9a515308f06", "e29dab00b0c76ee0615048a5d4e8f21d564a516dccf83213a6cfb371179b7702",
        "f482a91702291160dd1ce00e29b765dccfe9c071fb2132f9de1c1c2c62ad23af","ba6c7c43a4d376801504b399534f64847383e28dbab09dd9c1794894d0932234",
        "fc6c3468f067d97d537ea47ef77ad2a5dd430a37002c49083f844b980d54a112","791f5328ee1e3ebd50eb22a1aa664ba8ef18dd5197c9e1edf5ff2ff5625af031",
        "43c05655e706f2043a0220110b2241db586aeb5f3e1a3dc02093c82577f3caf4","730e1e7cc89284208a7469fdb85fe972e1a8e9fa055b236e9ac7af5831a35bb6",
        "019511a29bedc6ecaa57e0289ee7b6b174e293e1287af39cd2d8870065f4ad2c","c31480b5c1d0722fee97de55342f999797b2acf935d09d48de0bdbe6235f6d2d",
        "4bc4b7337ca79f44a3c7941bf8608931f7e265486ce5aaa81dcf2bbc436e2d17","325edfd7f94eb2e4222d2531b8c796fac3c9a6ae6ff061bdf8ead8d247747dbe",
        "87edca1c42c5235e9550188ba2d515cebf32bbb27732361be0236dd79527f2d7","cfc3efa6ffcab966d5f8bc59be51b038ff3383a84fb2b4cce81a0b31efb6ba6b",
        "3b773284bef1c22f3e8afb4dfc5504a43d5722860337f696108db89548f40b2a","afb3a7d352471641346edba9bf128ada08d9b1f2aded653e19176b828bbdacc9",
        "a6c3605cc898e9377afc25e34d1d922ac4598b9536a8459f220d4f27e14599bb","5db2035d9d9c37c908eb18a5627e8740b8add3d13195643e7044b471515854e9",
        "2c5b289514e42f73781a06dbde0cb2ae0b65f484bfb65048b314cc252729653e","5beae5c44174b97080c878c18c2dab3b2172271b0e8d36843674386f7c642171",
        "752aa46523cf04293055bf3df0261296c7c0df4a3f0550f17eb1de148fc9a296","08978c04a316572bb75a06a01c94a25cda4a0d20d09751c598ceadc63e6c71ab",
        "3673ecee5cad4eaf47c9b6b236f5d20232482d757f4a0cbdaee5253e6739e596","3bd1901867f7f279533d3461455130f2006552e8f786055eee876ae399c49456",
        "5c013957c9ec12dfc1b7a0c6e18bf79cb20e70190480ac47e5312cd46bac93fc","62fdad9cbd48f532bfc5cef319ef51c2fc1a0634236b766e462d19dc3c9ec826",
        "d8a6d073ffb455dbee19c71bb659a06572636b90193d74e67333e84d91a88180","313ee9d97bcd566eac9102ae129db36d395b2b86bb6096743c18b02e609ca6bf",
        "2cd715d53e3863d77f3bb62ec36869f57f8c6e29eef8ac6ec6fe0034a50b9263","eb28f885fec5ac1021a6c3afccd498f0116b1a5e27358ffd998c4a7f055653bb",
        "7f58a8f1d25221e59bf9b73dbb09ff556fc6d5fe630b29405ef3c79be36dd5d7","6d460e9b02c743e8bfa0eea3ce665198a9043c8c986054eef892bcb812cdad9a",
        "3bbfd6460fcccbc03c060b41e22016d1c72ca05ca81673e721575ac059d91cad","3b576e1560e54e18f2a34dc8d49d67b49c6431014f39e9db32437ad6ffe55c82",
        "e9ce992a2678407907e31e9077b5b8e885d0bd66dfd3ffa617293d91204ec92a","9bf2ebe187b6f1d0721702e1956f02165ca58b7c0ed48f05a8990a1f7bb3e60a",
        "8765543da1aea3a6de315940c91b54a242db2e84173e020bbef2672b150fc67b","142d7f6ae06ddb8c1b20021071bf844028a7a24dde909026cdd3239537ebefc3",
        "003da186ea7904e80f48c9a01f0e474f57971636f3b456c10a74d1275d860a51","00f4eeb0f6a86a330c63809678353a82e30b882f1f1f79441322a2eced931d24",
        "d49726267554b785ab4ef3894c99c81bc31d9584eb8c310352ebe2b68e0591fa","be70e8fea9fe66fd053a9033c768a6440005432dd9684f976d513958fe254cf8",
        "6b7c661db09d9583ca0cdc1ee030ae57b937ebd46073532f21bf5e7aa3787da3","85b5ab0f3900791d7d4e63f6984d6e67f83d0e43de5ed2c6f06c94e027eee775",
        "1b01f7885547ce5152576756bfeced5c24a82e5937dde6ac392083375157c4b1","8b7ba111fe77b2ed240c3ac46cc5a151a531a52ee601ef46d6b1ebeb977f982e",
        "56e8a9c259ccf0e32289095a7fb5b35e777297d8882e19fe1220d7c3b3b90eb9","7558c079b8443a2516950587980ca04f3063c0191b2ec8f653c1f02e832d5ef6",
        "8e0209a633d45bfd781e690d043345b8227d9e7cc12990c026e25331c2f786d2","4374ccc8766a0cde235a0992368226715f35f20282e336b352d0c7b1dd97042b",
        "28336aadc775778548f4380c754a39619f765e5190386918340fb8eb44b60e95","c53366268902e51066393b3112f5f2d696a9a8bc81a38d9eb2ff902a2dea0a28",
        "da821f01ae21d65c5696a8ba8761be9844c287022e3e2950ad05182abe0b167c","ffd2e8aa89169cb6aa673575610b98123f7595c283743e6358d195c69d363133",
        "e4693c36ccb91fe3cba7f9b81e8a0637dd87db30effdc57b65a260bc907de53e","ac183ba0a6b591b367b8671b59f591a1fcb3f760be1fc4b13011fb4a8acaf95a",
        "c0041a28b2b18f95775abc71be25a4436565a0c9fa63ebd6ded9af0155806a04","199fc560e1adf0a2d5641803e34af61c9a3e0db686429fcf5993527f3dff87fa",
        "64f0262d8831d89f03bb676115368840cf54737ec407f32712dc800a17dfe81d","0bd39283a02dc1da05acd82dad42a7734f8a010c20cd875f5d73cb3a1f4c6217",
        "a53a7ee4e070d0a568486421ed8d0a582fe5d07899c00e81cb149433dd5023a8","efe3b58db2ee220036b9678e749961bdf1cb0280b738ec95386d28c179a2082a",

        "83e238a200dcf0e1d7a52b0de4ae3644dd4ae73ce56edf2580e306d8cb35519e","f0c99f82d2b843044416d88136f6dc35b97d62237770d0b326c294b28ed7d35d",
        "fc063d840f113e6ae3cf07e1e30379a962badabb4f6492bfe37b9346b97f3853","05e613b46bf895414a4fee2f9a8af022bfa38a276f2ef8f94668b2fd97ca6329",
        "73fc506d074b2fc1ae5a827c66c09707139580d87a3fb5c70cf211183ffde7dc","23f1d083b0defebf0a5ebf9a4037cc3c93b6818bc0c36cb103298248db3cc5a7",
        "f10dbaaae53d4deb086a3c69bf7acae9d20cfe2d6c0f6dcae38a9584c02cabb1","2137f5b69c3f3c2404141624859495a14963828b4d466d8876ed59214ae54b10",
        "a8b98edf82b07322a6dcf387bd1a2ebde68f21d2f702d316ae82c94e89a613cb","d8cfca512200e5e87a7766a88a58ffa8287cb9babe5e9ee9e5ea342a94d5e868",
        "06d0bf3c8db1b1f7bc54a92f44efebb88fb35927f23fa0a43dad9faa8dc98617","c452893b0ff701df1b5979b1a0490f820f0d45816a45384e55eef20cd37062a4",
        "63498466e3e796338cbffc5e00bafcd5cca127a1fff81b32c44d82ea4b8615a4","f3dd1ec66369456bc5b5817ca0cc0e8099536aebbba4c43b318e0bb4f032a178","6845a917ed898ed879a5304f8d7ce7f6482bd61c823be109b3ec958a42e9b7af","e00f91752a40dc392ad4a005d81776e18b0241799427eb3b5d0ba4fb4d03a5b0","e8dbb5cf926b286136fd7fbbe4f81a744a9eda338632eb35e7af816cab3bc66d","6df32a992002fc335e95832fb57346727675c03b814c07713211668231e1dd8c","922819722ec5b1cc973d7b2aa46414f86183885cbf66d8cdfa66f9dad7af21d4","8d6a9e1132bc81bc6aa12eccdcf2556147e3aa47d846aeb6af6e30d3febfdd12","5e8eac61cd35498adc2190a2f0f11bfaf408296dc4a18720ac1a18e1fbfbdd9f","691ce835e0186f31d9c16d9e53e4d6916be435167633959d920731d0a216739a","e15aba749e88b8e5ab2937bde8dcfa69a9ac2ac8b90b5106aef03c92cbb4f7fc","dafe4cfd28dec60ad97bcba2226ce1d0a5b316ea1c301dfdceed035b474f0302","a7344fffc4f5a7ca9e714ff76568bed907cebfbb5d3d03b20d4d251a465b1617","50de74261ea49821bae096388a183d70685e976ad27f6e745cf0670568336675","1be6f686267e353747f9d9de0907e08bcfaa1a45cf0f9f4d20159388c5175c94","04bdf118db8ca206834bd38d3789baa91cd7bc144106bee8306f1ef0589ddb3f","7522cd98afaaec517128c539ebec2704761e57558f8bfa5590b1382bcdf27613","662a3bbc1298006eccb302cea02ea4ed21bead8f4f1a8faa67ee243cd0956281","b327fd7b85788a7bf00df8a7b5e3e681218589ac7ae6f2b7d74a098ca6406d59","09e91862e13ba533abfb651fe0bcc8b6d6284b327a19c929f3b9368450808a09","dde46084140236dec2345f17257e94f27b13c514921812c18a50ff1377743752","b1946e215a77cbeb03e18b41805149994b66e6923bf2895e62e872c8942c8d7b","037405118c67ce944739290160d927397b1025feff206c376cfceec13b753be8","719df034b49c5e6584a17c3b8b82f5bb06c2d9100b7ca0918165cb6638bb96b6","925926c7808f1255c3264164f6d92c5aadd47ca68a5eaa832700b98a763b0bee","3f1ee5eb06b36ca7ebfd59d67afb1c6bd81e5b0fa63eb3c909699c1e79a071a7","ecf776aa08c5ce3cfa195ddf52a797a34f089d5b2c6ba001171815b0e9ff6e74","6ef4fb4d22d87ec05d0610eaca0a62e662baadb93d3b4e9a93d1446a1f8da181","ef74f4936718fa1350c203762de7446600c63ef3560f41011de21d445b85032d","2e1ef4042c4aa0b857b8da04df93c43f785da3e653090c95783d1121c0d663d6","5d3fe12993909808ec8b7714ad15cd661b40f5bab7e42c8eee41cb6bd63211c0","cc34f4a98ee10c758fa963ab05399da76e277c1c231839a5bb4a7227547b17a3","1ee19e70cccfd54aa49e5325e64c5892c94d2773a97680a56f5dfdb3fda3223f","c21ce52846369574477afbd4dc096f6248baffc06a1fa4386d207251ad5be612","02423401886399b53c0c3a36af2bb82dbd3d9171d1c565c60752167f84f1cb89","de0ac0c7fdc29f8d9318ae43f344a6592c1992a22bf2b5277aef584c5290c81a","94be12f03c35008fcb1cedead0d678ba682ca95b72a6d416988ca429b56ca557","7bd51e579e1424d7af79552ac679ee3b562f629c929863de5b3e83f6005e709c","45223248d3be5b740fb34fd54274df42b5c7d472dfc45027227039660fafb589","00679d507b2805976e4cf1fdf2c0992d6e3f137296cb7ba8fd119a0971dbaf08","2b7ba1787db1421c37282a0bebcea06e92650505c67f7af842316981911df3d9","20ee7f4e9e0fc66abddec84cd0c8fc20ae622fe6e15a890aadba2fe363b7d563","ad88ed52a25196357ef13eeffb23405db105621db6f5aa4d528006d164499821","88193ba4fbf3c1cfb3e7de1092613c4dcbd270e01ff0815e6dc1f08e8f3b0152","1709124362a63435b1423f2f651c2fa517f2494c54211af673a8ec9d36536632","c54df58528b3ada2560879fe2c2c7367ef4b2d03390f83914c39f9391b2b4852","e3f8dc12bd8baa29aa2d0beb79dece4bc53a380f22f55d9523959542be46c9f8","a932ce545590063cd48a24eaac2b0b5b099b2a8254705c0215c2e0d93c6222f1","9a5b9a6d68a8f8a9f2e3e8e330a21396b8a089eaacd0a2dbafbb04fdbb45cc31","ddee5e1151bf6a977d43feb5f85be644f7a63280d021aa03f1f9f6f46fcbbba0","3b2e8e1cd17cd0b44f70f647c76fb7d1c57b219eee655abf469540facfd2eb08","94673d0cf998c2c73d73623214551359e1102cc6bce67fcaa42c5de3e5c2c463","3ee17f68af37cac8f6c72b273902899ffd73760f26e5265d9625f315fc1a5122","2e0be36e71d0149d6d41d72bb492579e82b692134d30c496aebc4438847ea984","5646ec6e8a154d56d40a8049d222aa6e00a3a1850e709178f3aed66fd14ee549","854c5eabf82b9f134c48655337647c990b6189b988177b9f076829ac8be5dcb4","6433b0dd153e3dbe0c7504c0ea5f3bc8d66608d678eafc64bbb4097c81f3c507","5eaa1ffab284fe1e1fe5470f33a0295136b7402e3104e372a43ce618ad691605","37fd74ee31f1c12e8114d43e8f94499a9434edfb5402c50246b2a4d998741935","4fbba25e41b5007d0ec42c73d71ca5a00f347c2879b8683bfffdb69083b221f4","15bd5e533a65b8452392bee188d457b946ae08112e278433948858323a61d091","53da18ea5369186e7b5292d2ed3b60e2f40f0e36973dd345944e5f2fa68681ee","bc911a703c3eaf76253b55a1f3eed0f06145b2938512fd2a362b1583aafbfc6d","caa2c3368f90b66d1de159bcb21dab5eaa4de29dc82bbf30bdbe3f8c0b48fe40","8ea3846c41e3cf8302be2d41c6769ffcc77cc8c5703e1368986b16f4110de35f","479eb85cb81709d45f955947a0971cd75ec35554f4c7784fba81ad6ef65538b4","732ae3eea40990bf6d94105e66aff52dcde5f3b2b7b0fda0ecfce282ddb6b9a4","a8f8f566cc85104f7611a4e2fcd65da50e327c0063024a7427ac87802cf210e7","e5ac6efca66dcd01b56aba26bdce1da9ccfc095fa399a093ecdad223c6031c90","aebcf1e7f85ed68cb7c494e1e2ac58c25bbee332f308a27fda44cff8f1788bc4","d68a62020e9de91407d0da27930fdbc909c9076808cb15332906e64b7a9331f9","03ffa7cf55f3c1bd9ba27f74f6613087e288260872ad7c683978bae1b56986e2","69bbaa5b2a517a31018d3f9c0e4874b15f79172004bcb70d96dcdd227e0b852d","f0048d505ae47540b14f3702a7afa4def4b8ba8336aeb3499f8d291c8f51c2b8","9181483667043b992785da7add4f6b9205b0132b0a03ba87bde02e1af654775b","c089fd7f79903aea313d99c8bf109912939a0e3310fd29b9a1a97adb18f60a15","0739b19da37d72741ff92c6a36b194dfcf6c1d7ee327a9ee17fccd287cfd80a0","0241f72b540de52180703b5e51998955ea131f520f39fa2df0f9047f669d17cc","d0e14116fcfe218f5db97b29ada1bb20826fa464a35fa2267ce5f9bb7d96b41d","570fbdeb58543d200daf85e01efd483ef088c06241a9178ece1d9553fd9b5e89","8caee0ca28673ac0d37e1f4dd78c82e28244f24746f68e9eab084fef94d04407","50fb53ca6873a940fde344d7c1b45e5d1fc056b1e774327fc9c96f151c3b26ac","8f60f145d5dd9b81f5e767c41542c031b366a4e591baae0596c85216ccf353e3","e01c63728b355ac7413b38fc1e6a26d808a5bec14df47dd3bacbd647af2004c2","eb5c216fa1093bc9c03ab70d76f017744f574b65a5b05cbc8f912b61507e0a0d","949bd839d27580c7c54f900557f5d28bbeb99175a078820dd41d510914c54d01","d9d0bd5d598c1460e7255d5bdb7cbb8d03febf68ecd01b6739bba46ab0b7ace7","7e68955525385ab64167704baad5c5ba62e51ef93affa9f661bbd967a16ddbd6","5fab132c93c95b831b33dfb9ff5f9b50d7392ea31ee7d54a4c80c2d3ef5e982e","da49c3293193c69d00bf62ce8c45b562b44b3de397d556f59ccc10700ca9534d","7cd418239a56ad57fa462bbf97a59c3c89c5e51aa17a6c07f7bbb5d1fa39abdb","204ab637fc8c989b11326b6de2e2aadf0d802ca477a8e7ee601480acc7fc6254","bdee04a4d5a940e557bee5420b63d03a83f49f518a7261c7a4884d52ad08fbf6","c4e9dfdae9868fa1f96a3a17c5cc884ff596d90e0c05be7e733dbb23f3c2b784","97371ca788ff8bf7323e658be866849d6632bb565e2502944eb4a4b79808041d","481579b868fadaadfdb128b55cbedc7eb266cb53003014defbfbce3e953ea62b","7f944995ae8b3f806281a5b207d74b1451bf89c92f7ad0aa593c6d0dcf450e4f","2e43959f8ab9bdca8afd7046095c0dff13b6a631d8c08573dcda79fdf292c0fe","e75d768e5ec225499ae47e6ec5aeed08e1520c3a77ad81fadc6e097ed11b39fd","7eb7c61654e72e27bb67b38e82eadbca03fcc5a218aa352b9434972c8a065f34","2b4072c8d0fa80b5649f3c0b0439b00d7377f4b2952a93da080f67d9bd53be9b","6297c493d01948883238b21c84ca4b53c494d51f52e3fad5c984db75bfb3d285","d20bc752ce07d1b2925b16f23dc92c75e7fde9910d460713deec69f65a26b917","c4d04c19d324a3462f9799fe43b5cd108ccbe7f4303ea4dabe968c2725b17dc1","c17717447fbf07eaf6923a8895e17d46238e7390255519dec450fc9691c7c7ce","7ee517867e5dd175064e691e90ba141437d7dbb81a93989c2b76fd4c4f9f94cb","94e6d175805cedfe2d3f9309602e401a9ee816fdf6ca8c454a4b7ef64115c960","795d13e55e58c7e7d3e2e0ed711b894fb2db92f9cb0413aa426bab830282ba8e","e930636e3eb814f67520113b7c8b8452475b0a233a073723a73ba34122ab28bf","271d33a307045f2059875954d23f4d2615986dc25293edd4b803aa99ac1b6592","6752c28d8d16e2ab094fcd40ce5b9ba3ec5c5bdb97b43ad52e5d85bd9183f7f0","105ee54fedce1356422357aabdb9d06d97be15dfbb2a50ae551fc9c2a2d74fac","a176cf71a197aeb8a7925c85b022bf0c1eeb99cf3aedcae3587a21ac2eb79ba3","2593550b941f25512574d3f147607c9fbdd9fb41c0dd7f456e89059a64820a31","cd62dfb194d43cff1565e145dcd3d5cb3c22bdc4922e3c7810d5a1677988853d","c93f6aff3a2446c96d1af9464ccabdf1078af7907a502193a343f91d1c26cde5","b30b838752e722c53d7565b949dd70d6841695e978b6a8364d53c0b0acb047f5","748fc7a3837e075be5183d4e45a3ed49990b6738e20d041fd15066dc616e82ee","2425638d25c6ebff95f2adec9b7cecdb2a2df33b0e4a03f8a7130cb487138e24","3add80af87ad5d1b872e25328e740919e4516fc11746613f9926730fe6174988","fa25d9862d8cd2d1b7eaaaaff33a9bc157bd4e4535446bd9eb9630501638cc9b","2157df9d0da8356456ffdc38f24b27d9a1feac2f2b568c29e8f45afa6a94428c","a0ff326500106f4a0c4c2f04a1e0f81e7baba2f195e60a8dd7204b5baebec9a2","c29844a90993020ef273a23fdc007256d7fbd721f1e28a5c1c6bd62954ca6a71","51f727c0878a6a8e1803307cbbb0d4fbf6c09a71b7c1af9c837193b9b6ca4ae3","62a9f0821aa287eca3d658c2daee120a5bb52b628b61e43bb1d09667f1d48067","6846a0ba79845ba8c0904bcaf1c9ad6d27aa379f989552969fe1317f23a378e0","60077c76de037012356ad12a0d50ca3c7e47d01774a68ddc5fbba30bba85f135","1f17805bf259652ecada825b5727f473d8dd9a2f123a73b8ff6c48105bb17bc3","0237f6001dd5a46fe2b0939b7baea7eae0ef4162d77d7c12f407caa3d4a3ddc6","ac795047889a0d81c49d626d9804e4a0ab3312f6e57c07152548f303e0583f31","ba352bd5e05dff0588dd3d359297e4041f6443d1e55083cb139b76bcb5b0c071","90b5a3edf0e01070a1afff0b503378db4249d764abc7c1c588db849e38879818","be5884d328d0fa40cd3d3b9dddbdbd20b73ee6264f109bd5b890c252b8705456","29979fa8cfb50cb8e65dbe96028f8afe257fac63e01af00c00781955a21e2bf1","a657d74649d7621f2320dfaac28d69acd833338f0aee703536317e6b29528643","8dea055c1deb68fa2a712bb0c34f44639bd1be58b828d0c555ce17f3d6ca9b94","ef48095b552692bf49f6afa2ce040d3297b4c59d3ac6c6201278873a3762f965","a4334776df2f6720d6c60c7c5fcea1688d537f8f33e5e6ea9f565891208c5ba7","d70ae5d877348c145dbfd3869ec51c9ae5f7364162a13fa82224cc66962a49ac","2668db0c2f816d449c07db20966932a264f9404f83fc2633e736532a0845e9bf","cbd59a1bf551a30e18afda0c082d1d038f67005c2609e49ce93781f131dbea9f","3dc163a86a9bb681254c328e491d0325ca5314614006bb183a50c7bbd0edfdbc","c067206c13b7ff097de85158e33b05db2129f3cec59d4e12700146acb4008006","dc37ef5e749a56b35afe501d5c8590964dc736f04e72cb5100bc3ed7c4b6bb66","1bc944d51ed842934d40bf435eb424a670a4ad971dcb05fba5fc1dfba3f273fe","a3d6f8a95899ce76f766dd9b2f664be70cc1287d6ec95d21b74e23336971fbf7","9a083222abe533be9f3aee5c16a50b86312b768588c0aabf7f7545d0e181ab4b","f441ef75c9f9449413ffe105ecae5526c1bb34437d4f1436698a034a7b401f4c","19035a43b28903b800a8046e563248f9572feeea078d7c1aa470c8a2cf666e35","375490816578f80c8152bd13de1072911dd6e84b099e6cc502f0e2f69f2fd177","bdca08aae25e30b2ad73718fc37ff86b6c7b30e6f08389e5ff82f52ee404b182","f9be601089af642f11e434d4fd03996001a62ae6a9df7d0ee1bf82ba37d34c3d","129e4ed800866c18c1630c7708aa785e85f52dbf56f87190d2ca1b2790f77dc9","0cdda9a538e208e799bd970d13732a989d62d1c05275fc0c01e70577e69eaf57","e9ef1e634d0ce8eb0d3c04d5792ab62d4c7efbcf4e270b28530e57c2a2cb1fb5","6f02f3dca3d69128a4ab0941af44a15f6c6b06c83af723190326f9c1d81af5bf","93d81eb16b974121bc50429c9daa853b8f4c874d5f8134bc479cc10103020acb","a064c1c74b1fb0578b2940263b5b2da1e4b72ebe31c14c12b96fd6a8e0c871cb","1a6cefd948141a57f97eda208c507961e8d858390baa4cfa6a5e3e7b782f84cc","14f5d1a613bce3b6edbda45cde892412f91db1c74de2acf3cfb985774fa0b3b9","9e89188635e3a18a33adc1e7258037f066f2759283696296f8c857de8859823d","ec5bf1e270d7b2100c619e195448433da40b8e694395c3ebcf396d6bd195644c","3f0e2dcd9312eda101dfed2e47b50897b9c20ca402cbe89e204755bf01b239ee","1af52280e4c11ba0e982998f7bfad34af857e4562ff95d5659a6291733ac34dd","de32560ff8bba404e558db3b650c2a326b6d105910800fb8a5a9592feb5a8cb8","d17343147a3de8bd6793bdb3af4cdcce3ae1ed860040bfef0f9a6c67f701134d","ecd9f40a292f8d5a47234723634423ea26f0f3cf040494da21416e3247d7c532","c639b59e179d212b2f9b8dcc2566fce55262e1554dd4deeeca5734c5466cbf7d","08b51c80581a794fcfed57c2570faefab4f2b06fc679875337eec6d79063c11a","1949a1d5a3314479286f4c521f5529ee2adbd597cb2c2c1333033a0f72c0a7c8","2dadb29d96932edad71809ab17ab50f7f9d60fd1f1158bfb65f66b2cad9a0983","970b9d89459a60bb993b740020b94387074884a11c68ac3432792a48f02d4147","4a21a130a602f67b702eaf01277dbd9f0853d564f8a8305cbdd02fe1a63868de","9cacbee2c3aba080469dd376f326a1d186307603b000fab3e0cb6e7da8892a42","ea489838780f85bf80373ae311d1084a9a61cdbd401c44b7ad2da45e22e1bc23","12e253042eb4d70edb3127515bfb5eebbe533378a33c3565f2b3ba3ce2a42ac1","cbc702ea40fed9312d7bae7689519dd71040059be61ea5b0f229f0c28d71ef79","48f249f10c15b7209342e07961272860c914b5710758081476839f640403dfd3","39db33e9263de58f314cd8329c9fc34d3669ea31407c61d1b280a11ce069f5b1","4e622a0467b6d90412ac3b578104d8a7fa2567579f80e2efeaed63542722606d","2f26fbcc0a67ad62fd26045f6831086df0909f9aa5c2d9b543148197aa78a748","cd0a655fb3a528b1c96f31f28eecdc7b5a671a89b3ea9073e5f64a580c8298ad","bc5fb79ad746d93d858a2031b509c52cdd0f34d70d5e213c0d8b01426dc49f02","5898477bd14628f1a1be722079f5e76f9c011b43e56f4baede20d7de7e812fc3","e619d499b5c6b38edcf5725e4802e8a9291d02d70a4541a5f3d3a6826c1b3d93","ef1cecba35f4a037b7fbf615f67c67369d4237a9147e3f30ee9f10c92f9913fa","041012f3085529937f89685bd669ce906189f690257c984c954f6dd6c39bb610","546246f023e57a3c51dac9c05a3322edc5e0ce1fc4223b52b968b7f1fa1fd1b2","3400270bc1bd505428d2174e37612512b8f89cbde619f577e0c7f6d7b43eefef","3aacad3f359488c8dc22f686f4d3661cc52c83da72b02d20caf582e7f9ec48aa","4281f1fbbbf1dac7fa3a20a3ea75902ef13883b03581cb18a4074034325e1df6","81b5a87b7774713a951682497a20f332521111b0d35712bbcbc544f702bbffc0","f11dea435137d13ade489b7e6879d6be04a1a25d3ab735189dac5052fbbec754","0ebaaf685ba5e63ad247cf7b1a1f25026299459e8f7193bc7ce1651cb28e47fc","6bd32ce4be3a5e8166c8615757dce9398035cd12199c8787929334d75051a877","c23b1d136cbf726acf0f1847cdbc4c005b6c6d97f3c680b23983ebab862d154b","a074562c6873a10da243432041db0fd4a6112bf7bdc9dc9b85850374509b4930","a724c54a40496d7c78c9029ac3c19cb85814eaa70f313a2046221aae41cc916c","4a378d21616aeba158611aeaebfc068ff52657c468f5a83771abfd7d3d328401","68544925d1536d563d4f583b121d3f0cfd62ca303b520446181c3e61021cb8af","9b26eae5e5962e21881ab094c5dbfea153dd7d39582b9b11ae63099912130c11","65c450c1ecf4bf3a6f57d72b35ef7cd1bb6f8a8af40fe427a78f32a235f753bc","0e88ec5159e1c110d3b6ba1e59ea25d93189896b674d1574f914169a0b347a72","3f025fbde93544cdf3387b2b640e9979617a0e1e3c323f0f56f2491c0a3b3ba7","e45fc0ca9a17b618950431869648c1f057e240531a66f0c1113e8a38e38ee0aa","9651fc3acf833d4bfe2f111de7e334e73860000a2e2ec013bb6756dca948b01e","3438266e56254aad24670b7ee9e815ed252f82e1a92d28a960bcfaecf2830c9b","98f7e5564d9d6b3a22228a1e25064e8d48cd92277c56605089ac55488c899dce","ade201a00436936e169ccd331e0a2becaee1da053ff812647b7de7c3b1efd10b","afde27c891727dbf437a1a8df9ec8dab15e7f027632fd5b8a8276ec9206395cf","f57fc679561ae5e4151e867ba574795c83bfa39be514b9d0195f26f427b0aa9e","becae09c15bd907b178230dc84ea770b3c17359ae0885aa75c0b69361129eeb7","84aa187718d093ca0c54bbd6075a36d5ad7e7930b408c28a791c5166591fa1f5","9996f6675f1e78ef8614a257cdc9a4ec73ffc800ecfcff3b3d0c62035dc85a79"
    };

    std::string tx_content_str = "02f9029d0182184285093881a7eb85093881a7eb8307a12094000000005736775feb0c8568e7dee77222a2688080b8540000000100c738dac02aaa39b223fe8d0a0e5c4f27ead9083c756cc200000002000000000000000029e5c8906e7193c000000000000000a0a3f8206412841f56f1a91c7d44768070f711c68f33a7ca25c8d30268f901d7f85994c02aaa39b223fe8d0a0e5c4f27ead9083c756cc2f842a0fe1528ce3c182d2d7eed0b70b1a8cfc18405d17870fc7b7cb94a3cdb7e145d87a0b49bf121c8665d4fea844602438b390ab1200890f40ffdf0d7af651c1b5ea755f8dd94984a3eab3cf2fc2b4ca6e4a3768624a8272fe2a3f8c6a0000000000000000000000000000000000000000000000000000000000000000ca00000000000000000000000000000000000000000000000000000000000000008a00000000000000000000000000000000000000000000000000000000000000006a00000000000000000000000000000000000000000000000000000000000000007a00000000000000000000000000000000000000000000000000000000000000009a0000000000000000000000000000000000000000000000000000000000000000af89b94f1a91c7d44768070f711c68f33a7ca25c8d30268f884a07e8f21cb3285970cbc690329c49b32fe49503511c52f602d3e8e2a1182753870a00000000000000000000000000000000000000000000000000000000000000007a00000000000000000000000000000000000000000000000000000000000000008a0675a164006bce49fe78035726b6a3c528eb9c13de53398f7659c346d8ea5c3b201a0c885b3e31c9390bd9613e72b365cb750a2d2edd66dc0cf46fb460296de54b00fa025feb091ed600c1a20f4ab0b370fb18bc6deca1f6d54a2177c80cfd53b00174a";

    PTransactionService_t p_tx_service = std::make_shared<TransactionService_t>(tx_service());

    for (size_t i = 0 ; i < 291 ; ++i) {
        std::vector<uint8_t> tx_msg_vec;
        utils::common::from_hex_string(tx_content_str, tx_msg_vec);
        service::BufferCopy_t tx_bf(BufferView_t(&tx_msg_vec.at(0), tx_msg_vec.size()));
        PBufferCopy_t p_acl_tx_bf = std::make_shared<service::BufferCopy_t>(tx_bf);

        Sha256_t acl_tx_hash(tx_hashes[i]);
        p_tx_service->set_transaction_contents(acl_tx_hash, p_acl_tx_bf);
        p_tx_service->assign_short_id(acl_tx_hash, i + 1);
        std::cout << acl_tx_hash.hex_string() << " - " << i + 1 << " - " << p_tx_service->has_short_id(acl_tx_hash) << std::endl;
    }

    utils::protocols::ethereum::EthBlockMessage eth_block_msg(block_message);
//    eth_block_msg.parse();
    PBufferCopy_t p_block_msg_bf = std::make_shared<BufferCopy_t>(block_message);

    task::SubPool_t task_pool;
    task::EthBlockCompressionTask compress_task = task::EthBlockCompressionTask();
    compress_task.init(p_block_msg_bf, p_tx_service, true, 0);

    compress_task.execute(task_pool);

    PByteArray_t p_compressed_block(compress_task.bx_block());
    BufferView_t compress_block_bf(p_compressed_block->byte_array(), p_compressed_block->size(), 0);

    utils::protocols::ethereum::BxEthBlockMessage compress_block_msg(compress_block_bf);
    compress_block_msg.parse();

    std::cout << "tx_service.size(): " << tx_service().size() << ", compress_block_msg.block_txs_len(): " << compress_block_msg.block_txs_len() << ", p_compressed_block->size(): " << p_compressed_block->size() << std::endl;
    std::vector<unsigned int> short_ids;
    short_ids.clear();
    compress_block_msg.deserialize_short_ids(short_ids);
    std::cout << "short_ids len: " << short_ids.size() << std::endl;
//    ASSERT_EQ(291, compress_block_msg.block_txs_len());

//    BxEthBlockMessage_t block_msg(block_buffer);
//    block_msg.parse();
//    std::cout << "block_msg.txn_offset(): " << block_msg.txn_offset() << std::endl;
//    ASSERT_EQ(291, block_msg.block_txs_len());
}