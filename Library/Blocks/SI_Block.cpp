#include "SI_Block.h"

namespace mdf {

    enum SI_LinkDefinitions : int {
        SI_LinkComment = 2,
        SI_LinkName = 0,
        SI_LinkPath = 1,
    };

    static uint64_t constexpr size = sizeof(MDF_Header) + sizeof(SI_Links) + sizeof(SI_Data);
    static uint64_t constexpr linkCount = 3;

    //region Constructors

    SI_Block::SI_Block() : data({ 0 }), links({ 0 }) {
        // Initialize header.
        header.type = MDF_Type_SI;
        header.linkCount = linkCount;
        header.size = size;

        // Set manual file location to upper limit.
        fileLocation = UINT64_MAX;
    }

    SI_Block::SI_Block(const mdf::SI_Block &value) : MDF_Block(value), data(value.data), links({ 0 }) {
        // Create a deep copy.

        // Create copies of the links.
        blockLinks = std::vector<std::shared_ptr<MDF_Block>>(linkCount);
        setComment(createCopyIfSet(value.getComment()));
        setName(createCopyIfSet(value.getName()));
        setPath(createCopyIfSet(value.getPath()));

        // Re-link.
        postLoad();
    }

    //endregion

    //region Inherited functions

    bool SI_Block::load(std::istream &stream) {
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

    void SI_Block::save(std::ostream &stream) {
        // Let the base handle the initial saving.
        MDF_Block::save(stream);

        // Save links.
        if(blockLinks[0]) {
            links.tx_name = blockLinks[0]->fileLocation;
        }

        if(blockLinks[1]) {
            links.tx_path = blockLinks[1]->fileLocation;
        }

        if(blockLinks[2]) {
            links.md_comment = blockLinks[2]->fileLocation;
        }

        auto* dataPtr = reinterpret_cast<char*>(&links);
        stream.write(dataPtr, sizeof(links));

        dataPtr = reinterpret_cast<char*>(&data);
        stream.write(dataPtr, sizeof(data));
    }

    //endregion

    //region Getters
    std::shared_ptr<MD_Block> SI_Block::getComment() const {
        return std::dynamic_pointer_cast<MD_Block>(blockLinks[SI_LinkComment]);
    }

    std::shared_ptr<TX_Block> SI_Block::getName() const {
        return std::dynamic_pointer_cast<TX_Block>(blockLinks[SI_LinkName]);
    }

    std::shared_ptr<TX_Block> SI_Block::getPath() const {
        return std::dynamic_pointer_cast<TX_Block>(blockLinks[SI_LinkPath]);
    }

    //endregion

    //region Setters

    void SI_Block::setComment(std::shared_ptr<MD_Block> const& value) {
        blockLinks[SI_LinkComment] = value;
    }

    void SI_Block::setName(std::shared_ptr<TX_Block> const& value) {
        blockLinks[SI_LinkName] = value;
    }

    void SI_Block::setPath(std::shared_ptr<TX_Block> const& value) {
        blockLinks[SI_LinkPath] = value;
    }

    //endregion

}