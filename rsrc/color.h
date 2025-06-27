#pragma once
#include <cstdlib>
#include <iostream>
#include <rsrc/colormaps.h>
#include <array>
#include <string>

#define COLORMAP_SIZE sizeof(magma)
#define COLORMAP_CHAIN_SET_SIZE 7

struct Color {
    uint8_t r, g, b;
};

struct ColormapSet {
    int size;
    const _NormalizedColorF * const * set;
};

struct ColormapChain {
    int idx = 0;
    
    ColormapSet set;

    void next(bool* update_colomap_chain) {
        idx = (idx + 1) % set.size;
        *update_colomap_chain = true;
    }
    void back(bool* update_colomap_chain) {
        if (idx == 0) {
            idx = set.size - 1;
        }
        else {
            idx--;
        }
        *update_colomap_chain = true;
    }
    const _NormalizedColorF* get() const {
        return set.set[idx];
    }
};

constexpr const _NormalizedColorF* colormap_perceptually_uniform_sequential[5] = {
    viridis, plasma, inferno, magma, cividis 
};
constexpr ColormapChain colormap_chain_perceptually_uniform_sequential {
    .set = {
        .size = 5,
        .set = colormap_perceptually_uniform_sequential
    }
};

constexpr const _NormalizedColorF* colormap_sequential[18] = {
    greys, purples, blues, greens, oranges, reds, ylorbr, ylorrd, orrd, purd, rdpu, bupu, gnbu, pubu, ylgnbu, pubugn, bugn, ylgn
};
constexpr ColormapChain colormap_chain_sequential {
    .set = {
        .size = 18,
        .set = colormap_sequential
    }
};

constexpr const _NormalizedColorF* colormap_sequential_2[16] = {
    binary, gist_yarg, gist_gray, gray, bone, pink, spring, summer, autumn, winter, cool, wistia, hot, afmhot, gist_heat, copper
};
constexpr ColormapChain colormap_chain_sequential_2 {
    .set = {
        .size = 16,
        .set = colormap_sequential_2
    }
};

constexpr const _NormalizedColorF* colormap_diverging[12] = {
    piyg, prgn, brbg, puor, rdgy, rdbu, rdylbu, rdylgn, spectral, coolwarm, bwr, seismic
};
constexpr ColormapChain colormap_chain_diverging {
    .set = {
        .size = 12,
        .set = colormap_diverging
    }
};

constexpr const _NormalizedColorF* colormap_cyclic[3] = {
    twilight, twilight_shifted, hsv
};
constexpr ColormapChain colormap_chain_cyclic {
    .set = {
        .size = 3,
        .set = colormap_cyclic
    }
};

constexpr const _NormalizedColorF* colormap_qualitative[12] = {
    pastel1, pastel2, paired, accent, dark2 ,set1, set2, set3, tab10, tab20, tab20b, tab20c
};
constexpr ColormapChain colormap_chain_qualitative {
    .set = {
        .size = 12,
        .set = colormap_qualitative
    }
};

constexpr const _NormalizedColorF* colormap_misc[17] = {
    flag, prism, ocean, gist_earth, terrain, gist_stern, gnuplot, gnuplot2, cmrmap, cubehelix, brg, gist_rainbow, rainbow, jet, turbo, nipy_spectral, gist_ncar
};
constexpr ColormapChain colormap_chain_misc {
    .set = {
        .size = 17,
        .set = colormap_misc
    }
};

struct ColormapChainSet {
    int size;
    ColormapChain * chains;
};

struct ColormapChainChain {
    bool* update_colomap_chain;
    ColormapChainSet set;
    int idx = 0;
    
    void next_chain() {
        idx = (idx + 1) % set.size;
        *update_colomap_chain = true;
    }
    void prev_chain() {
        if (idx == 0) {idx = set.size - 1;}
        else {
            idx--;
        }
        *update_colomap_chain = true;
    }
    void next() {
        set.chains[idx].next(update_colomap_chain);
    }
    void back() {
        set.chains[idx].back(update_colomap_chain);
    }

    ColormapChain get() const {
        return set.chains[idx];
    }
};

/// @brief Builds and returns a `ColormapChainChain` object
/// @param update_colomap_chain A bool pointer that gets set to `true` whenever the colormap is updated
/// @return `ColormapChainChain` object
inline ColormapChainChain buildColormapChain(bool* update_colomap_chain) {
    static ColormapChain colormap_chains[COLORMAP_CHAIN_SET_SIZE] = {
        colormap_chain_perceptually_uniform_sequential,
        colormap_chain_sequential,
        colormap_chain_sequential_2,
        colormap_chain_diverging,
        colormap_chain_cyclic,
        colormap_chain_qualitative,
        colormap_chain_misc
    };


    return {
        update_colomap_chain,
        {
            .size = COLORMAP_CHAIN_SET_SIZE,
            .chains = colormap_chains
        }
    };
};

class ColormapChainNames {
public:
    static inline const std::array<const std::vector<std::string>, COLORMAP_CHAIN_SET_SIZE> colormap_chain_set_names {
        std::vector<std::string>{"Viridis", "Plasma", "Inferno", "Magma", "Cividis"},
        std::vector<std::string>{"Greys", "Purples", "Blues", "Greens", "Oranges", "Reds", "YlOrBr", "YlOrRd", "OrRd", "PuRd", "RdPu", "BuPu", "GnBu", "PuBu", "YlGnBu", "PuBuGn", "BuGn", "YlGn"},
        std::vector<std::string>{"Binary", "Gist_Yarg", "Gist_Gray", "Gray", "Bone", "Pink", "Spring", "Summer", "Autumn", "Winter", "Cool", "Wistia", "Hot", "Afmhot", "Gist_Heat", "Copper"},
        std::vector<std::string>{"PiYG", "PRGn", "BrBG", "PuOr", "RdGy", "RdBu", "RdYlBy", "RdYlGn", "Spectral", "Cool Warm", "Bwr", "Seismic"},
        std::vector<std::string>{"Twilight", "Twilight Shifted", "HSV"},
        std::vector<std::string>{"Pastel 1", "Pastel 2", "Paired", "Accent", "Dark 2", "Set 1", "Set 2", "Set 3", "Tab 10", "Tab 20", "Tab 20B", "Tab 20C"},
        std::vector<std::string>{"Flag", "Prism", "Ocean", "Gist_Earth", "Terrain", "Gist_Stern", "GnuPlot", "GnuPlot 2", "CMRmap", "CubeHelix", "Brg", "Gist_Rainbow", "Rainbow", "Jet", "Turbo", "Nipy_Spectral", "Gist_Ncar"}
    };

    static inline const std::array<std::string, COLORMAP_CHAIN_SET_SIZE> colormap_chain_names {
        "Perceptually Uniform Sequential",
        "Sequential",
        "Sequential 2",
        "Diverging",
        "Cyclic",
        "Qualitative",
        "Miscellaneous"
    };

    static inline const std::string& get_colormap(const ColormapChainChain& chain) {
        return colormap_chain_set_names[chain.idx][chain.get().idx];
    }

    static inline const std::string& get_colorchain(const ColormapChainChain& chain) {
        return colormap_chain_names[chain.idx];
    }
};