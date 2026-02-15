<script lang="ts">
  export let value: string;
</script>

<select id="painting-ratio" bind:value>
  <optgroup label="Square">
    <option value="ONE_ONE">1:1</option>
    <option value="TWO_TWO">2:2</option>
    <option value="THREE_THREE">3:3</option>
    <option value="FOUR_FOUR">4:4</option>
  </optgroup>

  <optgroup label="Wide">
    <option value="TWO_ONE" selected>2:1</option>
    <option value="THREE_TWO">3:2</option>
    <option value="FOUR_TWO">4:2</option>
    <option value="FOUR_THREE">4:3</option>
  </optgroup>

  <optgroup label="Tall">
    <option value="ONE_TWO">1:2</option>
    <option value="TWO_THREE">2:3</option>
    <option value="TWO_FOUR">2:4</option>
    <option value="THREE_FOUR">3:4</option>
  </optgroup>
</select>

<style></style>
