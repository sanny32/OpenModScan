// Screenshot modal functionality
document.addEventListener('DOMContentLoaded', function() {
    const modal = document.getElementById('imageModal');
    const modalImage = document.getElementById('modalImage');
    const prevBtn = document.getElementById('modalPrev');
    const nextBtn = document.getElementById('modalNext');
    const screenshots = Array.from(document.querySelectorAll('.screenshot-item img'));

    let currentIndex = 0;

    function showModal(index) {
        currentIndex = index;
        modalImage.src = screenshots[currentIndex].src;
        modalImage.alt = screenshots[currentIndex].alt;
        modal.classList.add('active');
        document.body.style.overflow = 'hidden';
        updateNavButtons();
    }

    function updateNavButtons() {
        prevBtn.classList.toggle('hidden', currentIndex === 0);
        nextBtn.classList.toggle('hidden', currentIndex === screenshots.length - 1);
        modalImage.style.cursor = 'pointer';
    }

    // Add click event to all screenshots
    screenshots.forEach((screenshot, index) => {
        screenshot.addEventListener('click', function() {
            if (window.innerWidth <= 768) return;
            showModal(index);
        });
    });

    prevBtn.addEventListener('click', function(e) {
        e.stopPropagation();
        if (currentIndex > 0) showModal(currentIndex - 1);
    });

    nextBtn.addEventListener('click', function(e) {
        e.stopPropagation();
        if (currentIndex < screenshots.length - 1) showModal(currentIndex + 1);
    });

    // Click on image — go to next screenshot, or close on last
    modalImage.addEventListener('click', function(e) {
        e.stopPropagation();
        if (currentIndex < screenshots.length - 1) showModal(currentIndex + 1);
        else closeModal();
    });

    // Close modal when clicking outside the image
    modal.addEventListener('click', function(e) {
        if (e.target === modal) closeModal();
    });

    // Keyboard navigation
    document.addEventListener('keydown', function(e) {
        if (!modal.classList.contains('active')) return;
        if (e.key === 'Escape') closeModal();
        if (e.key === 'ArrowLeft' && currentIndex > 0) showModal(currentIndex - 1);
        if (e.key === 'ArrowRight' && currentIndex < screenshots.length - 1) showModal(currentIndex + 1);
    });

    function closeModal() {
        modal.classList.remove('active');
        document.body.style.overflow = '';
    }
});
