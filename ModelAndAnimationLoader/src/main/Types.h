#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include <set>
#include <stdexcept>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace mal
{
    class ModelHandler;
    class AnimationHandler;
    class AnimationPlayer;
    class Loader;

    struct Configuration
    {
        std::vector<std::string> mSupportedFileExtensions;
    };

    typedef enum SceneElements
    {
        NONE      = 0,
        ANIMATION = 1,
        CAMERA    = 2,
        LIGHT     = 4,
        MATERIAL  = 8,
        MESH      = 16,
        SKELETON  = 32,
        TEXTURE   = 64,
    } SceneElements;

    namespace model
    {
        struct Vertex;
        struct Texture;
        struct Material;

        using VertexArray = std::vector<Vertex>;
        using IndexArray = std::vector<uint32_t>;
        using TextureArray = std::vector<Texture>;
        using MaterialArray = std::vector<Material>;

        constexpr uint32_t MAX_BONE_INFLUENCE = 4;
        constexpr uint32_t MAX_BONE_WEIGHTS   = 4;

        struct Texture
        {
            uint32_t mWidth = 0;
            uint32_t mHeight = 0;
            uint32_t mChannels = 4;
            std::vector<uint8_t> mPixels;
        };

        struct Material
        {
            glm::vec4 mBaseColor{ 1.0f };
            float mMetallic = 1.0f;
            float mRoughness = 1.0f;

            int32_t mBaseColorTextureId = -1;
            int32_t mNormalTextureId = -1;
            int32_t mMetallicRoughnessTextureId = -1;
            int32_t mOcclusionTextureId = -1;
            int32_t mEmissiveTextureId = -1;
        };

        struct Vertex
        {
            Vertex()
            {
                std::fill_n(&mBoneIds[0], MAX_BONE_INFLUENCE, -1);
                std::fill_n(&mBoneWeights[0], MAX_BONE_WEIGHTS, 0);
            }

            glm::vec3 mPosition{};
            glm::vec3 mNormal{};
            glm::vec3 mTangent{};
            glm::vec3 mBitangent{};
            glm::vec2 mTexCoord{};
            glm::vec4 mColor{};
            int mBoneIds [MAX_BONE_INFLUENCE];
            float mBoneWeights [MAX_BONE_WEIGHTS];
        };

        struct VertexWeight
        {
            uint32_t mVertexId = 0;
            float mWeight = 0;
        };

        struct Bone
        {
            int32_t mBoneId = -1;
            glm::mat4 mOffsetMatrix{};
        };

        struct Mesh
        {
            uint32_t mVertexOffset  = 0;
            uint32_t mVertexCount   = 0;
            uint32_t mIndexOffset   = 0;
            uint32_t mIndexCount    = 0;
            uint32_t mMaterialIndex = 0;
        };

        struct Node
        {
            glm::mat4 mTransform{};
            std::unordered_map<std::string, Mesh> mMeshes;
            std::unordered_map<std::string, Node> mChildNodes;
        };

        struct Scene
        {
            std::unordered_map<std::string, Node> mNodes;
            std::unordered_map<std::string, Bone> mBones;
        };

        class Model
        {
        public:
            const Scene& getDefaultScene() const
            {
                if (mScenes.contains("Scene")) return mScenes.at("Scene");
                throw std::runtime_error("Scene not found");
            }

            const Scene& getScene(const std::string& scene) const
            {
                if (mScenes.contains(scene)) return mScenes.at(scene);
                throw std::runtime_error("Scene not found");
            }

            const VertexArray& getVertices() const
            {
                return mVertices;
            }

            const IndexArray& getIndices() const
            {
                return mIndices;
            }

            const TextureArray& getTextures() const
            {
                return mTextures;
            }

            const MaterialArray& getMaterials() const
            {
                return mMaterials;
            }

            glm::vec3 getBoundsMin() const
            {
                return mBoundsMin;
            }

            glm::vec3 getBoundsMax() const
            {
                return mBoundsMax;
            }

            bool hasAnimations() const
            {
                return mSceneElements & ANIMATION;
            }

            bool hasCameras() const
            {
                return mSceneElements & CAMERA;
            }

            bool hasLights() const
            {
                return mSceneElements & LIGHT;
            }

            bool hasMaterials() const
            {
                return mSceneElements & MATERIAL;
            }

            bool hasMeshes() const
            {
                return mSceneElements & MESH;
            }

            bool hasSkeletons() const
            {
                return mSceneElements & SKELETON;
            }

            bool hasTextures() const
            {
                return mSceneElements & TEXTURE;
            }

        private:
            char mSceneElements = 0;

            glm::vec3 mBoundsMin;
            glm::vec3 mBoundsMax;

            std::unordered_map<std::string, Scene> mScenes;
            VertexArray mVertices;
            IndexArray mIndices;
            TextureArray mTextures;
            MaterialArray mMaterials;


            friend class mal::Loader;
            friend class mal::ModelHandler;
        };
    }

    namespace anim
    {
        struct PositionKey
        {
            glm::vec3 mPosition{};
            float mTimestamp;
        };

        struct RotationKey
        {
            glm::quat mRotation{};
            float mTimestamp;
        };

        struct ScaleKey
        {
            glm::vec3 mScale{};
            float mTimestamp;
        };

        struct Channel
        {
            std::vector<PositionKey> mKeyPositions;
            std::vector<RotationKey> mKeyRotations;
            std::vector<ScaleKey> mKeyScales;
        };

        struct Animation
        {
            float mDuration = 0;
            float mTicksPerSecond = 0;
            std::unordered_map<std::string, Channel> mChannels;
        };

        class AnimationSet
        {
        public:
            const Animation& getAnimation(const std::string& name) const
            {
                return mAnimations.at(name);
            }

        private:
            std::unordered_map<std::string, Animation> mAnimations;
            std::set<std::string> mBones;

            friend class mal::AnimationHandler;
            friend class mal::AnimationPlayer;
        };
    }

}
