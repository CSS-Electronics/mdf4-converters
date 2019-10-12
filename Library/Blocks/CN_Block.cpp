#include "CN_Block.h"

#include "CC_Block.h"
#include "CG_Block.h"
#include "MD_Block.h"
#include "SD_Block.h"
#include "SI_Block.h"
#include "TX_Block.h"

namespace mdf {

    enum CN_LinkDefinitions : int {
        CN_LinkComment = 7,
        CN_LinkComposition = 1,
        CN_LinkConversion = 4,
        CN_LinkData = 5,
        CN_LinkName = 2,
        CN_LinkNextChannel = 0,
        CN_LinkSource = 3,
        CN_LinkUnit = 6,
    };

    static uint64_t constexpr size = 68;
    static uint64_t constexpr linkCount = 8;

    //region Constructors

    CN_Block::CN_Block() : data({ 0 }), links({ 0 })  {
        // Initialize header.
        header.type = MDF_Type_CN;
        header.linkCount = linkCount;
        header.size = size;

        // Set manual file location to upper limit.
        fileLocation = UINT64_MAX;
    }

    CN_Block::CN_Block(const mdf::CN_Block &value) : MDF_Block(value), data(value.data), links({ 0 }) {
        // Create a deep copy. Thus, all set links will be copied.
        blockLinks = std::vector<std::shared_ptr<MDF_Block>>(linkCount);

        // Create copies of the links.
        setComment(createCopyIfSet(value.getComment()));
        setComposition(createCopyIfSet(value.getComposition()));
        setConversion(createCopyIfSet(value.getConversion()));
        setName(createCopyIfSet(value.getName()));
        setNextCN(createCopyIfSet(value.getNextCN()));
        setSource(createCopyIfSet(value.getSource()));
        setUnit(createCopyIfSet(value.getUnit()));

        MDF_Type unitType = value.getDataType();
        if(unitType == MDF_Type_CG) {
            setData(createCopyIfSet(value.getDataCG()));
        } else if(unitType == MDF_Type_SD) {
            setData(createCopyIfSet(value.getDataSD()));
        } else {
            setData(std::shared_ptr<CG_Block>());
        }

        // Re-link.
        postLoad();
    }

    //endregion

    //region Inherited functions

    bool CN_Block::load(std::istream &stream) {
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

        return !stream.fail();
    }

    void CN_Block::postLoad() {
        // If composite is not noe, register all composite components.
        std::shared_ptr<CN_Block> compositeBlock = getComposition();
        while(compositeBlock) {
            composite.push_back(compositeBlock);
            compositeBlock = compositeBlock->getNextCN();
        }
    }

    void CN_Block::save(std::ostream &stream) {
        // Let the base handle the initial saving.
        MDF_Block::save(stream);

        // Save links.
        links.cn_next = setLinkIfValidPointer(getNextCN());
        links.composition = setLinkIfValidPointer(getComposition());
        links.tx_name = setLinkIfValidPointer(getName());
        links.si_source = setLinkIfValidPointer(getSource());
        links.cc_conversion = setLinkIfValidPointer(getConversion());
        links.data = setLinkIfValidPointer(getDataGeneric());
        links.md_unit = setLinkIfValidPointer(getUnit());
        links.md_comment = setLinkIfValidPointer(getComment());

        auto* dataPtr = reinterpret_cast<char*>(&links);
        stream.write(dataPtr, sizeof(links));

        dataPtr = reinterpret_cast<char*>(&data);
        stream.write(dataPtr, sizeof(data));
    }

    //endregion

    //region Getters

    std::shared_ptr<MD_Block> CN_Block::getComment() const {
        return std::dynamic_pointer_cast<MD_Block>(blockLinks[CN_LinkComment]);
    }

    std::shared_ptr<CN_Block> CN_Block::getComposition() const {
        return std::dynamic_pointer_cast<CN_Block>(blockLinks[CN_LinkComposition]);
    }

    std::shared_ptr<CC_Block> CN_Block::getConversion() const {
        return std::dynamic_pointer_cast<CC_Block>(blockLinks[CN_LinkConversion]);
    }

    std::shared_ptr<MDF_Block> CN_Block::getDataGeneric() const {
        return std::dynamic_pointer_cast<MDF_Block>(blockLinks[CN_LinkData]);
    }

    std::shared_ptr<CG_Block> CN_Block::getDataCG() const {
        return std::dynamic_pointer_cast<CG_Block>(blockLinks[CN_LinkData]);
    }

    std::shared_ptr<SD_Block> CN_Block::getDataSD() const {
        return std::dynamic_pointer_cast<SD_Block>(blockLinks[CN_LinkData]);
    }

    MDF_Type CN_Block::getDataType() const {
        MDF_Type result = MDF_Type_Invalid;
        std::shared_ptr<MDF_Block> block = getDataGeneric();

        if(block) {
            result = block->header.type;
        }

        return result;
    }

    std::shared_ptr<TX_Block> CN_Block::getName() const {
        return std::dynamic_pointer_cast<TX_Block>(blockLinks[CN_LinkName]);
    }

    std::shared_ptr<CN_Block> CN_Block::getNextCN() const {
        return std::dynamic_pointer_cast<CN_Block>(blockLinks[CN_LinkNextChannel]);
    }

    std::shared_ptr<SI_Block> CN_Block::getSource() const {
        return std::dynamic_pointer_cast<SI_Block>(blockLinks[CN_LinkSource]);
    }

    std::shared_ptr<MD_Block> CN_Block::getUnit() const {
        return std::dynamic_pointer_cast<MD_Block>(blockLinks[CN_LinkUnit]);
    }

    //endregion

    //region Setters

    void CN_Block::setComment(std::shared_ptr<mdf::MD_Block> const& value) {
        blockLinks[CN_LinkComment] = value;
    }

    void CN_Block::setComposition(std::shared_ptr<mdf::CN_Block> const& value) {
        blockLinks[CN_LinkComposition] = value;
    }

    void CN_Block::setConversion(std::shared_ptr<mdf::CC_Block> const& value) {
        blockLinks[CN_LinkConversion] = value;
    }

    void CN_Block::setData(std::shared_ptr<mdf::MDF_Block> const& value) {
        blockLinks[CN_LinkData] = value;
    }

    void CN_Block::setName(std::shared_ptr<mdf::TX_Block> const& value) {
        blockLinks[CN_LinkName] = value;
    }

    void CN_Block::setNextCN(std::shared_ptr<mdf::CN_Block> const& value) {
        blockLinks[CN_LinkNextChannel] = value;
    }

    void CN_Block::setSource(std::shared_ptr<mdf::SI_Block> const& value) {
        blockLinks[CN_LinkSource] = value;
    }

    void CN_Block::setUnit(std::shared_ptr<mdf::MD_Block> const& value) {
        blockLinks[CN_LinkUnit] = value;
    }

    //endregion

    bool CN_Block::isComposite() const {
        return !composite.empty();
    }

    bool CN_Block::isVLSD() const {
        return data.cn_type & CN_TypeVLSD;
    }

}
