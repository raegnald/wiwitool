<script lang="ts">
  import { ChevronLeft, ChevronRight } from "@lucide/svelte";
  import type { Snippet } from "svelte";

  export interface CarouselItem {
    image?: string;
    alt?: string;
    content: Snippet;
  }

  let {
    items = [],
    intervalMs = 5000,
  }: {
    items: CarouselItem[];
    intervalMs?: number;
  } = $props();

  let currentIndex = $state(0);
  let isHovered = $state(false);

  function next() {
    currentIndex = (currentIndex + 1) % items.length;
  }

  function prev() {
    currentIndex = (currentIndex - 1 + items.length) % items.length;
  }

  function goTo(index: number) {
    currentIndex = index;
  }

  // Automatically slide every intervalMs, pausing on hover
  $effect(() => {
    if (!isHovered && items.length > 1) {
      const timer = setInterval(next, intervalMs);
      return () => clearInterval(timer);
    }
  });
</script>

<div
  class="carousel"
  onmouseenter={() => (isHovered = true)}
  onmouseleave={() => (isHovered = false)}
  role="region"
  aria-label="Features carousel"
>
  <div
    class="carousel-track"
    style="transform: translateX(-{currentIndex * 100}%);"
  >
    {#each items as item, i}
      <div class="slide" aria-hidden={currentIndex !== i}>
        {#if item.image}
          <div class="content-image">
            <img src={item.image} alt={item.alt || ""} />
          </div>
        {/if}
        <div class="content-text">
          {@render item.content()}
        </div>
      </div>
    {/each}
  </div>

  {#if items.length > 1}
    <button class="nav-btn prev" onclick={prev} aria-label="Previous slide">
      <ChevronLeft />
    </button>
    <button class="nav-btn next" onclick={next} aria-label="Next slide">
      <ChevronRight />
    </button>

    <div class="indicators">
      {#each items as _, i}
        <button
          class="indicator"
          class:active={currentIndex === i}
          onclick={() => goTo(i)}
          aria-label="Go to slide {i + 1}"
        ></button>
      {/each}
    </div>
  {/if}
</div>

<style>
  .carousel {
    position: relative;
    overflow: hidden;
    width: 100%;
    border-radius: 8px;
    padding: 10px 0 35px 0; /* padding for the bottom indicators */
  }
  .carousel-track {
    display: flex;
    transition: transform 0.5s ease-in-out;
    width: 100%;
  }
  .slide {
    min-width: 100%;
    display: flex;
    flex-direction: column;
    gap: 15px;
    padding: 10px 45px; /* space for arrows */
    box-sizing: border-box;
    align-items: center;
    justify-content: center;
  }
  .content-text {
    text-align: center;
    font-size: 1.05em;
    line-height: 1.5;
  }
  .content-image {
    display: flex;
    justify-content: center;
    width: 100%;
  }
  .content-image img {
    max-width: 100%;
    border-radius: 8px;
    box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
  }
  .nav-btn {
    position: absolute;
    top: calc(50% - 15px);
    transform: translateY(-50%);
    background: transparent;
    border: none;
    color: var(--text-color, inherit);
    font-size: 1.5rem;
    cursor: pointer;
    border-radius: 50%;
    width: 40px;
    height: 40px;
    display: flex;
    align-items: center;
    justify-content: center;
    transition: background 0.2s;
    z-index: 10;
  }
  .nav-btn:hover {
    background: rgba(100, 108, 255, 0.1);
  }
  .nav-btn.prev {
    left: 0;
  }
  .nav-btn.next {
    right: 0;
  }
  .indicators {
    position: absolute;
    bottom: 15px;
    left: 50%;
    transform: translateX(-50%);
    display: flex;
    gap: 8px;
  }
  .indicator {
    width: 10px;
    height: 10px;
    border-radius: 50%;
    background: #ccc;
    border: none;
    cursor: pointer;
    transition: background 0.3s;
  }
  .indicator.active {
    background: #646cff;
  }

  @media (prefers-color-scheme: dark) {
    .indicator {
      background: #555;
    }
    .indicator.active {
      background: #646cff;
    }
    .nav-btn:hover {
      background: rgba(255, 255, 255, 0.1);
    }
  }
</style>
