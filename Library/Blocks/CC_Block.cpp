#include "CC_Block.h"

namespace mdf {

    enum CC_LinkDefinitions : int {
        CC_LinkComment = 2,
        CC_LinkInverse = 3,
        CC_LinkName = 0,
        CC_LinkUnit = 1,
    };

    static uint64_t constexpr size = 68;
    static uint64_t constexpr linkCount = 4;

    //region Constructors

    CC_Block::CC_Block() : data({ 0 }), links({ 0 }) {
        // Initialize header.
        header.type = MDF_Type_CC;
        header.linkCount = linkCount;
        header.size = size;

        // Set manual file location to upper limit.
        fileLocation = UINT64_MAX;
    }

    CC_Block::CC_Block(const mdf::CC_Block &value) : MDF_Block(value), data(value.data), links({ 0 }) {
        // Create a deep copy. Thus, all set links will be copied.
        values = value.values;
        blockLinks = std::vector<std::shared_ptr<MDF_Block>>(linkCount);

        // Create copies of the links.
        setName(createCopyIfSet(value.getName()));
        setComment(createCopyIfSet(value.getComment()));
        setInverse(createCopyIfSet(value.getInverse()));

        MDF_Type unitType = value.getUnitType();
        if(unitType == MDF_Type_MD) {
            setUnit(createCopyIfSet(value.getUnitMD()));
        } else if(unitType == MDF_Type_TX) {
            setUnit(createCopyIfSet(value.getUnitTX()));
        } else {
            setUnit(std::shared_ptr<TX_Block>());
        }
    }

    //endregion

    //region Inherited functions

    bool CC_Block::load(std::istream &stream) {
        // Let the base handle the initial loading.
        bool result = MDF_Block::load(stream);

        if(!result) {
            return result;
        }

        // Skip the links section.
        uint64_t currentLocation = stream.tellg();
        stream.seekg(currentLocation + header.linkCount * sizeof(uint64_t));

        // Load the data fields.
        auto* dataPtr = reinterpret_cast<char*>(&data);
        stream.read(dataPtr, sizeof(data));

        // Load the values.
        values.reserve(data.cc_val_count);
        double val;
        dataPtr = reinterpret_cast<char*>(&val);

        for(int i = 0; i < data.cc_val_count; ++i) {
            stream.read(dataPtr, sizeof(val));
            values.emplace_back(val);
        }

        return !stream.fail();
    }

    void CC_Block::save(std::ostream &stream) {
        // Let the base handle the initial saving.
        MDF_Block::save(stream);

        // Save links.
        links.tx_name = setLinkIfValidPointer(getName());
        links.md_unit = setLinkIfValidPointer(getUnitGeneric());
        links.md_comment = setLinkIfValidPointer(getComment());
        links.cc_inverse = setLinkIfValidPointer(getInverse());

        auto* dataPtr = reinterpret_cast<char*>(&links);
        stream.write(dataPtr, sizeof(links));

        dataPtr = reinterpret_cast<char*>(&data);
        stream.write(dataPtr, sizeof(data));

        // Copy values.
        uint64_t bytesToWrite = sizeof(double) * data.cc_val_count;
        if(bytesToWrite > 0) {
            dataPtr = reinterpret_cast<char*>(values.data());
            stream.write(dataPtr, bytesToWrite);
        }
    }

    //endregion

    //region Getters

    std::shared_ptr<MD_Block> CC_Block::getComment() const {
        return std::dynamic_pointer_cast<MD_Block>(blockLinks[CC_LinkComment]);
    }

    std::shared_ptr<CC_Block> CC_Block::getInverse() const {
        return std::dynamic_pointer_cast<CC_Block>(blockLinks[CC_LinkInverse]);
    }

    std::shared_ptr<TX_Block> CC_Block::getName() const {
        return std::dynamic_pointer_cast<TX_Block>(blockLinks[CC_LinkName]);
    }

    std::shared_ptr<MDF_Block> CC_Block::getUnitGeneric() const {
        return blockLinks[CC_LinkUnit];
    }

    std::shared_ptr<MD_Block> CC_Block::getUnitMD() const {
        return std::dynamic_pointer_cast<MD_Block>(blockLinks[CC_LinkUnit]);
    }

    std::shared_ptr<TX_Block> CC_Block::getUnitTX() const {
        return std::dynamic_pointer_cast<TX_Block>(blockLinks[CC_LinkUnit]);
    }

    MDF_Type CC_Block::getUnitType() const {
        MDF_Type result = MDF_Type_Invalid;
        std::shared_ptr<MDF_Block> block = getUnitGeneric();

        if(block) {
            result = block->header.type;
        }

        return result;
    }

    //endregion

    //region Setters

    void CC_Block::setComment(std::shared_ptr<MD_Block> const& value) {
        blockLinks[CC_LinkComment] = value;
    }

    void CC_Block::setInverse(std::shared_ptr<CC_Block> const& value) {
        blockLinks[CC_LinkInverse] = value;
    }

    void CC_Block::setName(std::shared_ptr<TX_Block> const& value)  {
        blockLinks[CC_LinkName] = value;
    }

    void CC_Block::setUnit(std::shared_ptr<MDF_Block> const& value) {
        blockLinks[CC_LinkUnit] = value;
    }

    //endregion



}
