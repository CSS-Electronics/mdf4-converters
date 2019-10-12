#include "FH_Block.h"

#include <sstream>

#include "HD_Block.h"

namespace mdf {

    enum FH_LinkDefinitions : int {
        FH_LinkComment = 1,
        FH_LinkNextFH = 0,
    };

    static uint64_t constexpr size = sizeof(MDF_Header) + sizeof(FH_Links) + sizeof(FH_Data);
    static uint64_t constexpr linkCount = 2;

    //region Constructors

    FH_Block::FH_Block() : data({ 0 }), links({ 0 }) {
        // Initialize header.
        header.type = MDF_Type_FH;
        header.linkCount = linkCount;
        header.size = size;
    }

    FH_Block::FH_Block(const mdf::FH_Block &value) : MDF_Block(value), data(value.data), links({ 0 }) {
        // Create a deep copy.
        data = value.data;
        links = { 0 };

        // Create copies of the links.
        blockLinks = std::vector<std::shared_ptr<MDF_Block>>(linkCount);

        setComment(createCopyIfSet(value.getComment()));
    }

    //endregion

    //region Inherited functions

    bool FH_Block::load(std::istream &stream) {
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

    void FH_Block::save(std::ostream &stream) {
        // Let the base handle the initial saving.
        MDF_Block::save(stream);

        // Save links.
        if(blockLinks[1]) {
            links.md_comment = blockLinks[1]->fileLocation;
        }

        auto* dataPtr = reinterpret_cast<char*>(&links);
        stream.write(dataPtr, sizeof(links));

        dataPtr = reinterpret_cast<char*>(&data);
        stream.write(dataPtr, sizeof(data));
    }

    //endregion

    //region Getters

    std::shared_ptr<MD_Block> FH_Block::getComment() const {
        return std::dynamic_pointer_cast<MD_Block>(blockLinks[FH_LinkComment]);
    }

    std::shared_ptr<FH_Block> FH_Block::getNextFH() const {
        return std::dynamic_pointer_cast<FH_Block>(blockLinks[FH_LinkNextFH]);
    }

    //endregion

    //region Setters

    void FH_Block::setComment(const std::shared_ptr<mdf::MD_Block> &value) {
        blockLinks[FH_LinkComment] = value;
    }

    void FH_Block::setNextFH(const std::shared_ptr<mdf::FH_Block> &value) {
        blockLinks[FH_LinkNextFH] = value;
    }

    //endregion

}
