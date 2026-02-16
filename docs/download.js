// Fetch latest release information from GitHub
async function fetchLatestRelease() {
    const GITHUB_API = 'https://api.github.com/repos/sanny32/OpenModScan/releases/latest';

    try {
        console.log('Fetching latest release from GitHub...');
        const response = await fetch(GITHUB_API);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }

        const release = await response.json();
        updateDownloadLinks(release);
    } catch (error) {
        console.error('Error fetching release:', error);
        console.warn('Download links will point to releases page instead of direct downloads');
    }
}

function updateDownloadLinks(release) {
    const assets = release.assets;

    // Find specific file patterns
    const files = {
        winx64Qt6: assets.find(a => a.name.match(/qt6-omodscan-.*_x64\.exe$/i)),
        winx64Qt5: assets.find(a => a.name.match(/qt5-omodscan-.*_x64\.exe$/i)),
        winx86: assets.find(a => a.name.match(/qt5-omodscan-.*_x86\.exe$/i)),
        debQt6: assets.find(a => a.name.match(/qt6-omodscan_.*_amd64\.deb$/i)),
        debQt5: assets.find(a => a.name.match(/qt5-omodscan_.*_amd64\.deb$/i)),
        rpmQt6: assets.find(a => a.name.match(/qt6-omodscan-.*\.x86_64\.rpm$/i)),
        flatpak: assets.find(a => a.name.match(/io\.github\.sanny32\.omodscan.*\.flatpak$/i))
    };

    // Helper function to update link
    function updateLink(id, file, name) {
        const link = document.getElementById(id);
        if (link && file) {
            link.href = file.browser_download_url;
        } else if (link && !file) {
            console.warn(`✗ File not found for ${name}`);
        }
    }

    // Update all download links
    updateLink('win-x64-qt6-link', files.winx64Qt6, 'Windows x64 Qt6');
    updateLink('win-x64-qt5-link', files.winx64Qt5, 'Windows x64 Qt5');
    updateLink('win-x86-qt5-link', files.winx86, 'Windows x86 Qt5');
    updateLink('deb-qt6-link', files.debQt6, 'DEB Qt6');
    updateLink('deb-qt5-link', files.debQt5, 'DEB Qt5');
    updateLink('rpm-qt6-link', files.rpmQt6, 'RPM Qt6');
    updateLink('flatpak-link', files.flatpak, 'Flatpak');

    // Update version display if available
    if (release.tag_name) {
        updateVersionDisplay(release.tag_name);
    }
}

function updateVersionDisplay(version) {
    // Update the download subtitle to include version
    const subtitle = document.querySelector('.download-subtitle');
    if (subtitle) {
        subtitle.textContent = `Get the latest version (${version}) for your platform`;
    }
}

// Load release information when page loads
document.addEventListener('DOMContentLoaded', fetchLatestRelease);
